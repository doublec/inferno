/* TODO/BUGS in no order:
   should send acknowledgement when an sms is read from /phone/sms, not just on every read
   consider notifying RIL of screen state using RIL_REQUEST_SCREEN_STATE to conserve power
   too many assumptions made in loop_for_data() about read() giving exactly the right amount of data
   parcel code will go crazy if ints overflow
   need to support DTMF
*/

#include "dat.h"
#include "fns.h"
#include "error.h"
#include "version.h"
#include "mp.h"
#include "libsec.h"
#include "keyboard.h"
#include <stdio.h>
#include <cutils/sockets.h>
#include <stdlib.h>
#include <telephony/ril.h>
#include <cutils/jstring.h>
#include <arpa/inet.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include "smsutils.h"
#include "parcel.h"

// RIL-related definitions

#define RESPONSE_SOLICITED 0
#define RESPONSE_UNSOLICITED 1
#define POWER_ON 2000
#define POWER_OFF 2001

// Inferno-related globals

enum
{
	Qdir,
	Qctl,
	Qsms,
	Qphone,
	Qsignal,
	Qstatus,
	Qcalls
};

Dirtab phonetab[] =
{
	".",	  {Qdir, 0, QTDIR}, 0, DMDIR|0555,
	"ctl",	  {Qctl},	    0, 0666,
	"sms",	  {Qsms},	    0, 0666,
	"phone",  {Qphone},	    0, 0666,
	"signal", {Qsignal},	    0, 0666,
	"status", {Qstatus},	    0, 0666,
	"calls",  {Qcalls},	    0, 0666,
};

static int signal_strength;
static int fd;
static int power_state = 0;

static struct {
	QLock l;
	Rendez r;
	int ready;
	int used;
	char *msg;
} status_msg;

static struct {
	QLock l;
	Rendez r;
	int ready;
	int used; // cat will read until it gets 0 bytes from read()
	          // need to be able to give cat 0 bytes after it's received the
	          // data
	RIL_Call *cs;
	int num; // number of calls in cs
} calls;

static struct listener_node *phone_listeners = NULL;
static struct listener_node *sms_listeners = NULL;

static void loop_for_data(void *v);
static void send_sms(char *smsc_pdu, char *pdu);
static void dial(char *number);
static void activate_net(void);
static void deactivate_net(void);
static void radio_power(int power);
static void answer(void);
static void acknowledge_sms(void);
static void get_reg_state(void);
static void set_mute(int);
static void get_current_calls(void);
static void hangup(int);
static void hangup_current(void);
static void get_call_fail_cause(void);

// AudioFlinger layer stuff
static void *af_handle;

void (*af_setMode)(int);
void (*af_setVoiceVolume)(float);
void (*af_setParameters)(char *);
void (*af_test)(void);

enum audio_mode {
	MODE_INVALID = -2,
	MODE_CURRENT = -1,
	MODE_NORMAL = 0,
	MODE_RINGTONE,
	MODE_IN_CALL,
	MODE_IN_COMMUNICATION,
	NUM_MODES  // not a valid entry, denotes end-of-list
};

#define DPRINT if(1) print

// Function for Sleep() to check readiness with
static int
calls_ready(void *unused)
{
	return calls.ready;
}

// Function for Sleep() to check readiness with
static int
status_ready(void *unused)
{
	return status_msg.ready;
}

void
phoneinit(void)
{
	af_handle = dlopen("libinfernoaudio.so", RTLD_NOW);
	if(af_handle == NULL) {
		fprintf(stderr, "opening libinfernoaudio.so failed: %s\n",
			dlerror());
	}
	af_setMode = dlsym(af_handle, "af_setMode");
	af_setVoiceVolume = dlsym(af_handle, "af_setVoiceVolume");
	af_setParameters = dlsym(af_handle, "af_setParameters");

	if(!(af_setMode && af_setVoiceVolume && af_setParameters)) {
		fprintf(stderr, "could not load AudioFlinger functions");
		dlclose(af_handle);
	}

	af_setMode(MODE_NORMAL);

	calls.cs = NULL;
	calls.num = 0;
	calls.ready = 0;
	calls.used = 0;
	memset(&calls.l, 0, sizeof(QLock));

	status_msg.ready = 0;
	status_msg.used = 0;
	memset(&status_msg.l, 0, sizeof(QLock));

	// set up proper permissions (need to be user radio, group radio to
	// access socket)
	if(setegid(1001) == -1) {
		perror("setegid(1001) failed");
	}
	if(seteuid(1001) == -1) {
		perror("seteuid(1001) failed");
	}

	fd = socket_local_client("rild",
				 ANDROID_SOCKET_NAMESPACE_RESERVED,
				 SOCK_STREAM);
	if(fd == -1) {
		perror("socket_local_client failed");
	}

	// set privileges back to root/root
	if(seteuid(0) == -1) {
		perror("seteuid(0) failed");
	}
	if(setegid(0) == -1) {
		perror("setegid(0) failed");
	}

	kproc("phone", loop_for_data, 0, 0);
}

static Chan*
phoneattach(char *spec)
{
	// setup kprocs if necessary
	return devattach('f', spec);
}

static Walkqid*
phonewalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, phonetab, nelem(phonetab), devgen);
}

static int
phonestat(Chan *c, uchar *db, int n)
{
	return devstat(c, db, n, phonetab, nelem(phonetab), devgen);
}

static Chan*
phoneopen(Chan *c, int omode)
{
	c = devopen(c, omode, phonetab, nelem(phonetab), devgen);

	switch((ulong) c->qid.path) {
	case Qstatus:
		qlock(&status_msg.l);
		status_msg.used = 0;
		status_msg.ready = 0;
		get_reg_state();
		break;
	case Qcalls:
		qlock(&calls.l);
		calls.used = 0;
		calls.ready = 0;
		get_current_calls();
		break;
	case Qphone:
		c->aux = qopen(512, 0, nil, nil);
		add_listener(&phone_listeners, c->aux);
		break;
	case Qsms:
		c->aux = qopen(512, 0, nil, nil);
		add_listener(&sms_listeners, c->aux);
		break;
	}

	return c;
}

static void
phoneclose(Chan *c)
{
	if((c->flag & COPEN) == 0)
		return;

	switch((ulong) c->qid.path) {
	case Qphone:
		del_listener(&phone_listeners, c->aux);
		qfree(c->aux);
		c->aux = NULL;
		break;
	case Qsms:
		del_listener(&sms_listeners, c->aux);
		qfree(c->aux);
		c->aux = NULL;
		break;
	}
}

static long
phoneread(Chan *c, void *va, long n, vlong offset)
{
	char buf[255];
	int i, str_offset;
	if(c->qid.type & QTDIR) {
		return devdirread(c, va, n, phonetab, nelem(phonetab), devgen);
	}

	switch((ulong) c->qid.path) {
	case Qctl:
		return readstr(offset, va, n, power_state ? "on\n" : "off\n");
	case Qphone:
		return qread(c->aux, va, n);
	case Qsms:
		// FIXME only acknowledge when an SMS is being read
		acknowledge_sms();
		return qread(c->aux, va, n);
	case Qsignal:
		snprintf(buf, sizeof(buf), "%d\n", signal_strength);
		return readstr(offset, va, n, buf);
	case Qstatus:
		Sleep(&status_msg.r, status_ready, NULL);
		if(status_msg.ready == -1) {
			// an error occurred
			status_msg.used = 1;
			qunlock(&status_msg.l);
			return readstr(offset, va, n, "error\n");
		}
		if(status_msg.used) {
			status_msg.ready = 0;
			status_msg.used = 0;
			return 0; // already sent the data
		}
		status_msg.used = 1;
		return readstr(offset, va, n, status_msg.msg);
	case Qcalls:
		Sleep(&calls.r, calls_ready, NULL);
		if(calls.ready == -1) {
			// an error occurred
			calls.used = 1;
			qunlock(&calls.l);
			return readstr(offset, va, n, "");
		}
		if(calls.used || calls.num == 0) {
			calls.ready = 0;
			calls.used = 0;
			qunlock(&calls.l);
			return 0; // already sent the data
		}
		calls.used = 1;

		str_offset = 0;
		for(i = 0; i < calls.num; i++) {
			RIL_Call c = calls.cs[i];
			str_offset += snprintf(va + str_offset, n - str_offset,
					       "call %d:\n "
					       " state %d\n index %d\n toa %d\n"
					       " isMpty %d\n isMT %d\n als %d\n isVoice %d\n"
					       " isVoicePrivacy %d\n number %s\n"
					       " numberPresentation %d\n name %s\n"
					       " namePresentation %d\n",
					       i, c.state, c.index, c.toa, c.isMpty, c.isMT,
					       c.als, c.isVoice, c.isVoicePrivacy, c.number,
					       c.numberPresentation, c.name,
					       c.namePresentation);
		}
		qunlock(&calls.l);
		return str_offset;
	}
	return 0;
}

static long
phonewrite(Chan *c, void *va, long n, vlong offset)
{
	char *pdu;
	Rune *runestr;
	int i, j;
	Cmdbuf *cmd;

	if(c->qid.type & QTDIR)
		error(Eperm);

	cmd = parsecmd(va, n);
	if(waserror()) {
		free(cmd);
		nexterror();
		return 0;
	}

	switch((ulong) c->qid.path) {
	case Qctl:
		if(strcmp(cmd->f[0], "on") == 0) {
			radio_power(1);
		} else if(strcmp(cmd->f[0], "off") == 0) {
			radio_power(0);
		} else if(strcmp(cmd->f[0], "net") == 0) {
			if(cmd->nf >= 2) {
				if(strcmp(cmd->f[1], "on") == 0) {
					activate_net();
				} else if(strcmp(cmd->f[1], "off") == 0) {
					deactivate_net();
				}
			}
		} else if(strcmp(cmd->f[0], "mute") == 0) {
			set_mute(1);
		} else if(strcmp(cmd->f[0], "unmute") == 0) {
			set_mute(0);
		} else if(strcmp(cmd->f[0], "route") == 0) {
			if(cmd->nf >= 2) {
				if(strcmp(cmd->f[1], "earpiece") == 0) {
					af_setParameters("routing=1");
				} else if(strcmp(cmd->f[1], "speaker") == 0) {
					af_setParameters("routing=2");
				} else if(strcmp(cmd->f[1], "headphone") == 0) {
					af_setParameters("routing=3");
				}
			}
		} else if(strcmp(cmd->f[0], "volume") == 0) {
			if(cmd->nf >= 2) {
				switch(strtol(cmd->f[1], NULL, 10)) {
				case 0: af_setVoiceVolume(0.0); break;
				case 1: af_setVoiceVolume(0.2); break;
				case 2: af_setVoiceVolume(0.4); break;
				case 3: af_setVoiceVolume(0.6); break;
				case 4: af_setVoiceVolume(0.8); break;
				case 5: af_setVoiceVolume(1.0); break;
				default: af_setVoiceVolume(1.0); break;
				}
			}
		}
		break;
	case Qsms:
		if(strcmp(cmd->f[0], "send") == 0) {
			if(cmd->nf == 3) {
				runestr = malloc((utflen(cmd->f[2]) + 1) * sizeof(Rune));
				for(i = 0, j = 0; j < strlen(cmd->f[2]); i++) {
					j += chartorune(runestr + i, cmd->f[2] + j);
				}
				runestr[i] = 0;
				pdu = encode_sms(cmd->f[1], runestr);
				send_sms(NULL, pdu);
				
				free(runestr);
				free(pdu);
			}
		}
		break;
	case Qphone:
		if(strcmp(cmd->f[0], "answer") == 0) {
			answer();
		} else if(strcmp(cmd->f[0], "dial") == 0) {
			if(cmd->nf != 2) {
				fprintf(stderr, "malformed dial request\n");
				break;
			}
			dial(cmd->f[1]);
		} else if(strcmp(cmd->f[0], "hangup") == 0) {
			if(cmd->nf == 2) {
				hangup(strtol(cmd->f[1], NULL, 10));
			} else {
				hangup_current();
			}
		}
		break;
	}
	poperror();
	free(cmd);
	return n;
}

Dev phonedevtab = {
	'f',
	"phone",

	phoneinit,
	phoneattach,
	phonewalk,
	phonestat,
	phoneopen,
	devcreate,
	phoneclose,
	phoneread,
	devbread,
	phonewrite,
	devbwrite,
	devremove,
	devwstat
};

/* RIL functions */

// Get a string describing a call fail error from the call fail code
static char*
call_fail_str(int err)
{
	switch(err) {
	case CALL_FAIL_UNOBTAINABLE_NUMBER: return "unobtainable number";
	case CALL_FAIL_NORMAL: return "normal failure"; // no idea
	case CALL_FAIL_BUSY: return "busy";
	case CALL_FAIL_CONGESTION: return "congestion";
	case CALL_FAIL_ACM_LIMIT_EXCEEDED: return "ACM limit exceeded";
	case CALL_FAIL_CALL_BARRED: return "call barred";
	case CALL_FAIL_FDN_BLOCKED: return "FDN blocked";
	case CALL_FAIL_IMSI_UNKNOWN_IN_VLR: return "IMSI unknown in VLR";
	case CALL_FAIL_IMEI_NOT_ACCEPTED: return "IMEI not accepted";
	case CALL_FAIL_CDMA_LOCKED_UNTIL_POWER_CYCLE:
		return "locked until power cycle";
	case CALL_FAIL_CDMA_DROP: return "drop";
	case CALL_FAIL_CDMA_INTERCEPT: return "intercept";
	case CALL_FAIL_CDMA_REORDER: return "reorder";
	case CALL_FAIL_CDMA_SO_REJECT: return "SO reject";
	case CALL_FAIL_CDMA_RETRY_ORDER: return "retry order";
	case CALL_FAIL_CDMA_ACCESS_FAILURE: return "access failure";
	case CALL_FAIL_CDMA_PREEMPTED: return "preempted";
	case CALL_FAIL_CDMA_NOT_EMERGENCY: return "not an emergency number";
	case CALL_FAIL_CDMA_ACCESS_BLOCKED: return "access blocked";
	default: return "unknown";
	}
}

// This is called when RIL sends a response with a non-zero error code. It sends
// a message to be written to a device if needed so that whoever sent the 
// command can realize it didn't work.
static void
handle_error(int seq, int error)
{
	// RIL error messages
	char *errmsgs[] = { "error: no error\n", "error: radio not available\n", "error: generic failure\n", "error: password incorrect\n", "error: need SIM PIN2\n", "error: need SIM PUK2\n", "error: request not supported\n", "error: cancelled\n", "error: cannot access network during voice calls\n", "error: cannot access network before registering to network\n", "error: retry sending sms\n", "error: no SIM\n", "error: no subscription\n", "error: mode not supported\n", "error: FDN list check failed\n", "error: illegal SIM or ME\n" };
	char *errmsg = errmsgs[error];

	DPRINT("RIL sent error (seq %d): %s", seq, errmsg);

	switch(seq) {
	case RIL_REQUEST_SEND_SMS:
		listeners_produce(&sms_listeners, errmsg, strlen(errmsg));
		break;
	case RIL_REQUEST_DIAL:
		listeners_produce(&phone_listeners, errmsg, strlen(errmsg));
		// RIL sends a GENERIC_FAILURE error when the call fails
		// for non-radio reasons, e.g. a busy signal
		if(error == RIL_E_GENERIC_FAILURE) {
			// ask RIL what went wrong and send the result to the
			// device when we get the response later
			get_call_fail_cause();
		}
		break;
	case RIL_REQUEST_REGISTRATION_STATE:
		status_msg.ready = -1;
		Wakeup(&status_msg.r);
		qunlock(&status_msg.l);
		break;
	case RIL_REQUEST_GET_CURRENT_CALLS:
		calls.ready = -1;
		Wakeup(&calls.r);
		qunlock(&calls.l);
		break;
	}
}

/* handle_sol_response is called when we recieve a solicited response from RIL,
   i.e. the response to a request we sent. The response can contain data we
   asked for, indicate an error, or simply indicate success. */
static void
handle_sol_response(struct parcel *p)
{
	int seq, error, i, num, offset = 0;
	char buf[200];
	seq = parcel_r_int32(p);
	error = parcel_r_int32(p);
	DPRINT("received sol response: seq: %d err: %d\n", seq, error);
	if(error != 0) {
		handle_error(seq, error);
		return;
	}
	switch(seq) {
	case RIL_REQUEST_SETUP_DATA_CALL:
		if(!parcel_data_avail(p)) return;
		num = parcel_r_int32(p);
		for(i = 0; i < num; i++) {
			DPRINT("%s\n", parcel_r_string(p));
		}
		break;
	case POWER_ON:
		power_state = 1;
		break;
	case POWER_OFF:
		power_state = 0;
		af_setMode(MODE_NORMAL);
		break;
	case RIL_REQUEST_REGISTRATION_STATE:
		if(!parcel_data_avail(p)) return;
		num = parcel_r_int32(p);
		offset = 0;
		for(i = 0; i < num; i++) {
			offset += snprintf(buf + offset, sizeof(buf) - offset, "%s\n", parcel_r_string(p));
		}
		free(status_msg.msg);
		status_msg.msg = strdup(buf);
		status_msg.ready = 1;
		Wakeup(&status_msg.r);
		qunlock(&status_msg.l);
		break;
	case RIL_REQUEST_DIAL:
		DPRINT("got RIL_REQUEST_DIAL success\n");
		break;
	case RIL_REQUEST_GET_CURRENT_CALLS:
		if(!parcel_data_avail(p)) {
			calls.ready = 1;
			calls.num = 0;
			Wakeup(&calls.r);
			qunlock(&calls.l);
			break;
		}

		for(i = 0; i < calls.num; i++) {
			free(calls.cs[i].number);
			free(calls.cs[i].name);
			free(calls.cs[i].uusInfo);
		}
		free(calls.cs);
		num = parcel_r_int32(p);
		calls.num = num;
		calls.cs = malloc(num * sizeof(RIL_Call));
		if(calls.cs == NULL) {
			fprintf(stderr, "malloc failed while making %d RIL_Calls\n", num);
			calls.ready = -1;
			Wakeup(&calls.r);
			qunlock(&calls.l);
			break;
		}

		for(i = 0; i < num; i++) {
			calls.cs[i].state = parcel_r_int32(p);
			calls.cs[i].index = parcel_r_int32(p);
			calls.cs[i].toa = parcel_r_int32(p);
			calls.cs[i].isMpty = parcel_r_int32(p);
			calls.cs[i].isMT = parcel_r_int32(p);
			calls.cs[i].als = parcel_r_int32(p);
			calls.cs[i].isVoice = parcel_r_int32(p);
			calls.cs[i].isVoicePrivacy = parcel_r_int32(p);
			calls.cs[i].number = parcel_r_string(p);
			calls.cs[i].numberPresentation = parcel_r_int32(p);
			calls.cs[i].name = parcel_r_string(p);
			calls.cs[i].namePresentation = parcel_r_int32(p);
			calls.cs[i].uusInfo = parcel_r_int32(p) ?
					      malloc(sizeof(RIL_UUS_Info)) :
					      NULL;
			if(calls.cs[i].uusInfo != NULL) {
				// not implemented
				DPRINT("debug: got uusInfo\n");
			}
		}
		calls.ready = 1;
		Wakeup(&calls.r);
		qunlock(&calls.l);
		break;
	case RIL_REQUEST_LAST_CALL_FAIL_CAUSE:
		snprint(buf, sizeof(buf), "call fail cause: %s\n",
			call_fail_str(parcel_r_int32(p)));
		buf[sizeof(buf) - 1] = '\0';
		listeners_produce(&phone_listeners, buf, strlen(buf));
		break;
	}
}

/* handle_unsol_response is called when we receive an unsolicited response from
   RIL. Unsolicited responses inform us of events like an incoming call or
   SMS. */
static void
handle_unsol_response(struct parcel *p)
{
	int resp_type;
	char buf[300];
	int l;
	struct recvd_sms sms;

	resp_type = parcel_r_int32(p);
	switch(resp_type) {
	case RIL_UNSOL_RESPONSE_NEW_SMS:
		decode_sms(parcel_r_string(p), &sms);
		l = snprintf(buf, sizeof(buf), "%s (%s): %s\n", sms.src_num,
			     sms.timestamp, sms.msg);
		buf[l] = '\0';
		listeners_produce(&sms_listeners, buf, strlen(buf));

		free(sms.msg);
		free(sms.service_center);
		free(sms.src_num);
		free(sms.timestamp);
		break;
	case RIL_UNSOL_CALL_RING:
		listeners_produce(&phone_listeners, "ring\n", 5);
		break;
	case RIL_UNSOL_SIGNAL_STRENGTH:
		signal_strength = parcel_r_int32(p);
		// TODO: use bit error rate instead of cell tower reception
		// when available (during a call)
		break;
	case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED:
		listeners_produce(&phone_listeners, "call state changed\n", 20);
		break;
	}
}

// FIXME: read() needs to be handled better in this function, allow it to
// underrun and just loop again to get the rest of the data
static void
loop_for_data(void *v)
{
	seteuid(1001);
	for(;;) {
		int msglen, readlen, type, i, ret;
		char *buf;
		struct parcel p;

		parcel_init(&p);
		if((ret = read(fd, &msglen, sizeof(msglen))) != sizeof(msglen)) {
			perror("loop_for_data");
			fprintf(stderr, "EOF while reading parcel length\n");
			fprintf(stderr, "read returned %d\n", ret); 
			return;
		}
		msglen = ntohl(msglen);
		buf = (char *) malloc(msglen * sizeof(char));
		if(buf == NULL) {
			fprintf(stderr, "devphone:loop_for_data: malloc failed when attempting to allocate %d bytes\n", msglen);
			return;
		}
		readlen = read(fd, buf, msglen);
		if(readlen != msglen) {
			fprintf(stderr, "msglen = %d readlen = %d\n", msglen,
				readlen);
			fprintf(stderr,
				"got too much or not enough data, aborting\n");
			free(buf);
			return;
		}
		parcel_grow(&p, msglen);
		memcpy(p.data, buf, msglen);
		p.size = msglen;

		type = parcel_r_int32(&p);
		if(type == RESPONSE_SOLICITED) {
			handle_sol_response(&p);
		} else {
			handle_unsol_response(&p);
		}
		parcel_free(&p);
		free(buf);
	}
}

static void
send_ril_parcel(struct parcel *p)
{
	int pktlen, ret;

	// we write the length in bytes, then the parcel data
	pktlen = htonl(p->size);
	ret = write(fd, (void *)&pktlen, sizeof(pktlen));
	if(ret < 0) perror("write to ril failed");
	ret = write(fd, p->data, p->size);
	if(ret < 0) perror("write to ril failed");
}

static void
answer(void)
{
	struct parcel p;
	
	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_ANSWER); // request id
	parcel_w_int32(&p, RIL_REQUEST_ANSWER); // reply id

	send_ril_parcel(&p);
	parcel_free(&p);
}

static void
radio_power(int power)
{
	struct parcel p;

	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_RADIO_POWER); // request id
	parcel_w_int32(&p, power ? POWER_ON : POWER_OFF); // reply id
	parcel_w_int32(&p, 1); // ??? java code does it
	parcel_w_int32(&p, power); // 1 on, 0 off

	send_ril_parcel(&p);
	parcel_free(&p);
}

static void
dial(char *number)
{
	struct parcel p;
	struct mixer *mixer;
	struct mixer_ctl *ctl;

	af_setMode(MODE_IN_CALL); // tell audioflinger we want to start a call
	af_setParameters("routing=1"); // tell AF where to route sound
	af_setVoiceVolume(1.0f);
	set_mute(0);

	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_DIAL); // request id
	parcel_w_int32(&p, RIL_REQUEST_DIAL); // reply id
	parcel_w_string(&p, number); // number to dial
	parcel_w_int32(&p, 0); // clir mode
	parcel_w_int32(&p, 0); // 0 - uus info absent, 1 - uus info present
	parcel_w_int32(&p, 0); // 0 - uus info absent, 1 - uus info present
	                       // yes, twice...??

	send_ril_parcel(&p);
	parcel_free(&p);
}

static void
activate_net(void)
{
	struct parcel p;

	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_SETUP_DATA_CALL); // request ID
	parcel_w_int32(&p, RIL_REQUEST_SETUP_DATA_CALL); // reply id
	parcel_w_int32(&p, 7); // hardcoded in java
	parcel_w_string(&p, "1"); // CDMA or GSM. 1 = GSM
	parcel_w_string(&p, "0"); // data profile. 0 = default
	parcel_w_string(&p, "wap.cingular"); // APN name
	parcel_w_string(&p, "WAP@CINGULARGPRS.COM"); // APN user
	parcel_w_string(&p, "cingular1"); // APN passwd
	parcel_w_string(&p, "0"); // auth type. 0 usually

	send_ril_parcel(&p);
	parcel_free(&p);
}

static void
deactivate_net(void)
{
	struct parcel p;
	
	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_DEACTIVATE_DATA_CALL); // request id
	parcel_w_int32(&p, RIL_REQUEST_DEACTIVATE_DATA_CALL); // reply id
	parcel_w_string(&p, "1"); // FIXME: this should be the CID returned by
				  // RIL_REQUEST_SETUP_DATA_CALL response
	send_ril_parcel(&p);
	parcel_free(&p);
}

static void
send_sms(char *smsc_pdu, char *pdu)
{
	struct parcel p;

	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_SEND_SMS); // request id
	parcel_w_int32(&p, RIL_REQUEST_SEND_SMS); // reply id
	parcel_w_int32(&p, 2); // ??? java code does it
	parcel_w_string(&p, smsc_pdu); // SMSC PDU
	parcel_w_string(&p, pdu); // SMS PDU

	send_ril_parcel(&p);
	parcel_free(&p);
}

static void
acknowledge_sms(void)
{
	struct parcel p;

	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_SMS_ACKNOWLEDGE); // request id
	parcel_w_int32(&p, RIL_REQUEST_SMS_ACKNOWLEDGE); // reply id
	parcel_w_int32(&p, 2); // ??? java code does it
	parcel_w_int32(&p, 1); // success
	parcel_w_int32(&p, 0); // failure cause if necessary

	send_ril_parcel(&p);
	parcel_free(&p);
}

static void
get_reg_state(void)
{
	struct parcel p;
	
	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_REGISTRATION_STATE); // request id
	parcel_w_int32(&p, RIL_REQUEST_REGISTRATION_STATE); // reply id
	
	send_ril_parcel(&p);
	parcel_free(&p);
}

static void
set_mute(int muted)
{
	struct parcel p;
	
	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_SET_MUTE); // request id
	parcel_w_int32(&p, RIL_REQUEST_SET_MUTE); // reply id
	parcel_w_int32(&p, 1); // ??? java code does this
	parcel_w_int32(&p, muted); // 0 - unmute, 1 - mute

	send_ril_parcel(&p);
	parcel_free(&p);
}

static void
get_current_calls(void)
{
	struct parcel p;

	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_GET_CURRENT_CALLS); // request id
	parcel_w_int32(&p, RIL_REQUEST_GET_CURRENT_CALLS); // reply id

	send_ril_parcel(&p);
	parcel_free(&p);
}

static void
hangup(int index)
{
	struct parcel p;
	
	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_HANGUP); // request id
	parcel_w_int32(&p, RIL_REQUEST_HANGUP); // reply id
	parcel_w_int32(&p, 1); // ??? java code does it, don't know why
	parcel_w_int32(&p, index); // line index to hang up

	send_ril_parcel(&p);
	parcel_free(&p);
}

static void
hangup_current(void)
{
	struct parcel p;
	
	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND);
	parcel_w_int32(&p, RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND);

	send_ril_parcel(&p);
	parcel_free(&p);
}

static void
get_call_fail_cause(void)
{
	struct parcel p;
	
	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_LAST_CALL_FAIL_CAUSE);
	parcel_w_int32(&p, RIL_REQUEST_LAST_CALL_FAIL_CAUSE);

	send_ril_parcel(&p);
	parcel_free(&p);
}
