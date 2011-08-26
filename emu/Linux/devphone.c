/* TODO/BUGS in no order:
   should send acknowledgement when an sms is read from /phone/sms, not just on every read
   decode_sms is sloppy--should convert septets into octets, then convert octets into UTF-8 instead of trying to do it all at once. split off into separate functions?
   handle sending/receiving multiple text messages and checking length of messages sent to /phone/sms
   consider notifying RIL of screen state using RIL_REQUEST_SCREEN_STATE to conserve power
   too many assumptions made in loop_for_data() about read() giving exactly the right amount of data
   parcel code will go crazy if ints overflow
   no way to give an error if SMSes don't send
   need to support DTMF
   get calling number when ring event occurs (through CALL_STATE_CHANGED?)
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
#include <semaphore.h>
#include <dlfcn.h>
#include "asound.h"
#include "alsa_audio.h"
#include "smsutils.h"
#include "parcel.h"
// tmp
#include "audit.h"
#include <linux/ioctl.h>
#define __force
#define __bitwise
#define __user
#define AUDITCHECK 
//end tmp

// audio stuff taken from android source
#define AUDIO_HW_OUT_PERIOD_MULT 8 // (8 * 128 = 1024 frames)
#define AUDIO_HW_OUT_PERIOD_SZ (PCM_PERIOD_SZ_MIN * AUDIO_HW_OUT_PERIOD_MULT)
#define AUDIO_HW_OUT_PERIOD_CNT 4
// RIL-related definitions

#define RESPONSE_SOLICITED 0
#define RESPONSE_UNSOLICITED 1
#define POWER_ON 2000
#define POWER_OFF 2001

struct RilClient {
    void *prv;
};

typedef struct RilClient * HRilClient;

typedef enum _SoundType {
    SOUND_TYPE_VOICE,
    SOUND_TYPE_SPEAKER,
    SOUND_TYPE_HEADSET,
    SOUND_TYPE_BTVOICE
} SoundType;

typedef enum _AudioPath {
    SOUND_AUDIO_PATH_HANDSET,
    SOUND_AUDIO_PATH_HEADSET,
    SOUND_AUDIO_PATH_SPEAKER,
    SOUND_AUDIO_PATH_BLUETOOTH,
    SOUND_AUDIO_PATH_BLUETOOTH_NO_NR,
    SOUND_AUDIO_PATH_HEADPHONE
} AudioPath;

typedef enum _SoundClockCondition {
    SOUND_CLOCK_STOP,
    SOUND_CLOCK_START
} SoundClockCondition;

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

Queue *phoneq;
Queue *smsq;

static int signal_strength;
static int fd;
static int power_state = 0;
static struct {
	sem_t sem;
	char *msg;
} status_msg;

static struct {
	RWLock l;
	RIL_Call *cs;
	int num;
} calls;

/* RIL client library globals */
static void *ril_client_handle;
static HRilClient ril_client = NULL;
HRilClient (*openClientRILD)(void);
int (*disconnectRILD)(HRilClient);
int (*closeClientRILD)(HRilClient);
int (*isConnectedRILD)(HRilClient);
int (*connectRILD)(HRilClient);
int (*setCallVolume)(HRilClient, SoundType, int);
int (*setCallAudioPath)(HRilClient, AudioPath);
int (*setCallClockSync)(HRilClient, SoundClockCondition);


void loop_for_data(void *v);
void send_sms(char *smsc_pdu, char *pdu);
void dial(char *number);
void activate_net(void);
void deactivate_net(void);
void radio_power(int power);

void auditmixer_close(struct mixer *mixer);
struct mixer *auditmixer_open(void);

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

void phoneinit(void)
{
	af_handle = dlopen("libinfernoaudio.so", RTLD_NOW);
	if(af_handle == NULL) {
		fprintf(stderr, "opening libinfernoaudio.so failed: %s\n",
			dlerror());
	}
	af_setMode = dlsym(af_handle, "af_setMode");
	af_setVoiceVolume = dlsym(af_handle, "af_setVoiceVolume");
	af_setParameters = dlsym(af_handle, "af_setParameters");
	af_test = dlsym(af_handle, "af_test");

	printf("af_test = %p, af_setParameters = %p\n", af_test, af_setParameters);
	af_test();
	af_setMode(MODE_NORMAL);

	calls.cs = NULL;
	calls.num = 0;
	memset(&calls.l, 0, sizeof(RWLock));

	phoneq = qopen(512, 0, nil, nil);
	if(phoneq == 0)
		panic("no memory");
	smsq = qopen(512, 0, nil, nil);
	if(smsq == 0)
		panic("no memory");

	sem_init(&status_msg.sem, 0, 0);

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
	// Load RIL client functions, used for setting call volume, etc
/*	ril_client_handle = dlopen("libsecril-client.so", RTLD_NOW);
	if(ril_client_handle == NULL) {
		fprintf(stderr, "opening libsecril-client.so failed: %s\n",
			dlerror());
	}
	openClientRILD = dlsym(ril_client_handle, "OpenClient_RILD");
	disconnectRILD = dlsym(ril_client_handle, "Disconnect_RILD");
	closeClientRILD = dlsym(ril_client_handle, "CloseClient_RILD");
	isConnectedRILD = dlsym(ril_client_handle, "isConnected_RILD");
	connectRILD = dlsym(ril_client_handle, "Connect_RILD");
	setCallVolume = dlsym(ril_client_handle, "SetCallVolume");
	setCallAudioPath = dlsym(ril_client_handle, "SetCallAudioPath");
	setCallClockSync = dlsym(ril_client_handle, "SetCallClockSync");

	ril_client = openClientRILD();
	if(ril_client == NULL) {
		fprintf(stderr, "error in openClientRILD()\n");
	}
	if(connectRILD(ril_client) != 0) {
		fprintf(stderr, "error while attempting to connectRILD()\n");
		}*/
}

static Chan *phoneattach(char *spec)
{
	// setup kprocs if necessary
	return devattach('f', spec);
}

static Walkqid *phonewalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, phonetab, nelem(phonetab), devgen);
}

static int phonestat(Chan *c, uchar *db, int n)
{
	return devstat(c, db, n, phonetab, nelem(phonetab), devgen);
}

static Chan *phoneopen(Chan *c, int omode)
{
	c = devopen(c, omode, phonetab, nelem(phonetab), devgen);

	switch((ulong) c->qid.path) {
	case Qstatus:
		get_reg_state();
		break;
	case Qcalls:
		get_current_calls();
		break;
	}

	return c;
}

static void phoneclose(Chan *c)
{
	if((c->flag & COPEN) == 0)
		return;
}

static long phoneread(Chan *c, void *va, long n, vlong offset)
{
	char buf[255];
	if(c->qid.type & QTDIR) {
		return devdirread(c, va, n, phonetab, nelem(phonetab), devgen);
	}

	switch((ulong) c->qid.path) {
	case Qctl:
		return readstr(offset, va, n, power_state ? "on\n" : "off\n");
	case Qphone:
		return qread(phoneq, va, n);
	case Qsms:
		// FIXME only acknowledge when an SMS is being read
		acknowledge_sms();
		return qread(smsq, va, n);
	case Qsignal:
		snprintf(buf, sizeof(buf), "%d\n", signal_strength);
		return readstr(offset, va, n, buf);
	case Qstatus:
		sem_wait(&status_msg.sem);
		// terrible
		if(offset < strlen(status_msg.msg)) {
			sem_post(&status_msg.sem);
		}
		// FIXME lock
		return readstr(offset, va, n, status_msg.msg);
	case Qcalls:
		return readstr(offset, va, n, "fixme\n");
	}
	return 0;
}

static long phonewrite(Chan *c, void *va, long n, vlong offset)
{
	char *pdu;
	char *args[5];
	char *str;
	Rune *runestr;
	int nargs = 0, i, j;

	if(c->qid.type & QTDIR)
		error(Eperm);
	switch((ulong) c->qid.path) {
	case Qctl:
		str = auditmalloc((n + 1) * sizeof(char));
		strncpy(str, va, n);
		str[n] = '\0';
		printf("Qctl: va = %s\n", str);
		nargs = getfields(str, args, sizeof(args), 1, " \n");
		if(strcmp(args[0], "on") == 0) {
			radio_power(1);
		} else if(strcmp(args[0], "off") == 0) {
			radio_power(0);
		} else if(strcmp(args[0], "net") == 0) {
			if(nargs != 2) {
				break;
			}
			if(strcmp(args[1], "on") == 0) {
				activate_net();
			} else if(strcmp(args[1], "off") == 0) {
				deactivate_net();
			}
		}
		auditfree(str);
		break;
	case Qsms:
		str = auditmalloc((n + 1) * sizeof(char));
		strncpy(str, va, n);
		str[n] = '\0';
		printf("Qsms: va = %s\n", str);
		nargs = getfields(str, args, 3, 1, " ");
		if(strcmp(args[0], "send") == 0) {
			if(nargs != 3) {
				break;
			}
			runestr = auditmalloc((utflen(args[2]) + 1) * sizeof(Rune));
			for(i = 0, j = 0; j < strlen(args[2]); i++) {
				j += chartorune(runestr + i, args[2] + j);
			}
			runestr[i] = 0;
			pdu = encode_sms(args[1], runestr);
			send_sms(NULL, pdu);
			
			auditfree(str);
			auditfree(runestr);
			auditfree(pdu);
		}
		break;
	case Qphone:
		str = auditmalloc((n + 1) * sizeof(char));
		strncpy(str, va, n);
		str[n] = '\0';
		printf("Qphone: va = %s\n", str);
		nargs = getfields(str, args, sizeof(args), 1, " \n");
		if(strcmp(args[0], "answer") == 0) {
			answer();
			return n;
		} else if(strcmp(args[0], "dial") == 0) {
			if(nargs != 2) {
				fprintf(stderr, "malformed dial request\n");
				break;
			}
			dial(args[1]);
		}
		auditfree(str);
		break;
	}
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

void handle_error(int seq, int error)
{
	// RIL error messages
	char *errmsgs[] = { "no error", "radio not available", "generic failure", "password incorrect", "need SIM PIN2", "need SIM PUK2", "request not supported", "cancelled", "cannot access network during voice calls", "cannot access network before registering to network", "retry sending sms", "no SIM", "no subscription", "mode not supported", "FDN list check failed", "illegal SIM or ME" };
	char *errmsg = errmsgs[error];
	switch(seq) {
	case RIL_REQUEST_SEND_SMS:
		qproduce(smsq, errmsg, strlen(errmsg));
		break;
	}
}

void interpret_sol_response(struct parcel *p)
{
	int seq, error, i, num, offset = 0;
	char buf[200];
	seq = parcel_r_int32(p);
	error = parcel_r_int32(p);
	printf("received sol response: seq: %d err: %d\n", seq, error);
	if(error != 0) {
		handle_error(seq, error);
		return;
	}
	switch(seq) {
	case RIL_REQUEST_SETUP_DATA_CALL:
		if(!parcel_data_avail(p)) return;
		num = parcel_r_int32(p);
		for(i = 0; i < num; i++) {
			printf("%s\n", parcel_r_string(p));
		}
		break;
	case POWER_ON:
		power_state = 1;
		break;
	case POWER_OFF:
		power_state = 0;
		break;
	case RIL_REQUEST_REGISTRATION_STATE:
		if(!parcel_data_avail(p)) return;
		num = parcel_r_int32(p);
		offset = 0;
		for(i = 0; i < num; i++) {
			offset += snprintf(buf + offset, sizeof(buf) - offset, "%s\n", parcel_r_string(p));
		}
		// FIXME lock
		auditfree(status_msg.msg);
		status_msg.msg = auditstrdup(buf);
		sem_post(&status_msg.sem);
		break;
	case RIL_REQUEST_DIAL:
		printf("got RIL_REQUEST_DIAL success\n");
		break;
	case RIL_REQUEST_GET_CURRENT_CALLS:
		// DEBUG
		printf("current pos: %d\n", p->offset);
		for(i = 0; i < p->size; i++) {
			printf("%02x ", p->data[i]);
		}
		printf("\n");

		if(!parcel_data_avail(p)) {
			break;
		}

		wlock(&calls.l);
		printf("before free\n");
		for(i = 0; i < calls.num; i++) {
			printf("freeing number\n");
			auditfree(calls.cs[i].number);
			printf("freeing name\n");
			auditfree(calls.cs[i].name);
			printf("freeing uusInfo\n");
			auditfree(calls.cs[i].uusInfo);
		}
		auditfree(calls.cs);
		printf("after free\n");
		num = parcel_r_int32(p);
		printf("after read\n");
		calls.num = num;
		calls.cs = auditmalloc(num * sizeof(RIL_Call));
		printf("got past malloc\n");
		if(calls.cs == NULL) {
			wunlock(&calls.l);
			fprintf(stderr, "malloc failed while making %d RIL_Calls\n", num);
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
					      auditmalloc(sizeof(RIL_UUS_Info)) :
					      NULL;
			if(calls.cs[i].uusInfo != NULL) {
				// FIXME not implemented yet
				printf("debug: got uusInfo\n");
			}
		}
		wunlock(&calls.l);
		for(i = 0; i < calls.num; i++) {
			RIL_Call c = calls.cs[i];
			printf("call %d:\n state %d\n index %d\n toa %d\n"
			       "isMpty %d\n isMT %d\n als %d\n isVoice %d\n"
			       "isVoicePrivacy %d\n number %s\n"
			       "numberPresentation %d\n name %s\n"
			       "namePresentation %d\n",
			       i, c.state, c.index, c.toa, c.isMpty, c.isMT,
			       c.als, c.isVoice, c.isVoicePrivacy, c.number,
			       c.numberPresentation, c.name,
			       c.namePresentation);
		}
		break;
	}
}

void interpret_unsol_response(struct parcel *p)
{
	int resp_type;
	char buf[300];
	int l;
	struct recvd_sms sms;
	resp_type = parcel_r_int32(p);
//	printf("unsolicited: type = %d, data left = %d\n", resp_type,
//	       parcel_data_avail(p));
	switch(resp_type) {
	case RIL_UNSOL_RESPONSE_NEW_SMS:
		decode_sms(parcel_r_string(p), &sms);
		l = snprintf(buf, sizeof(buf), "%s (%s): %s\n", sms.src_num,
			     sms.timestamp, sms.msg);
		buf[l] = '\0';
		qproduce(smsq, buf, strlen(buf));

		auditfree(sms.msg);
		auditfree(sms.service_center);
		auditfree(sms.src_num);
		auditfree(sms.timestamp);
		break;
	case RIL_UNSOL_CALL_RING:
		printf("incoming call\n");
		qproduce(phoneq, "ring\n", 5);
		break;
	case RIL_UNSOL_SIGNAL_STRENGTH:
		signal_strength = parcel_r_int32(p) / 6; // signal strength from cell tower
		// TODO: use bit error rate instead of cell tower reception
		// when available (during a call)
		break;
	}
}

// FIXME: read() needs to be handled better in this function, allow it to
// underrun and just loop again to get the rest of the data
void loop_for_data(void *v)
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
		buf = (char *) auditmalloc(msglen * sizeof(char));
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
			auditfree(buf);
			return;
		}
		/*	for(i = 0; i < readlen; i++) {
			printf("%02x", buf[i]);
		}
		printf("\n");*/
		parcel_grow(&p, msglen);
		memcpy(p.data, buf, msglen);
		AUDITCHECK;
		p.size = msglen;
		//	printf("Received %d bytes of data from rild.\n", msglen);
		type = parcel_r_int32(&p);
		if(type == RESPONSE_SOLICITED) {
			interpret_sol_response(&p);
		} else {
			interpret_unsol_response(&p);
		}
		AUDITCHECK;
		parcel_free(&p);
		AUDITCHECK;
		auditfree(buf);
		AUDITCHECK;
	}
}

void send_ril_parcel(struct parcel *p)
{
	int pktlen, ret;
	pktlen = htonl(p->size);
	ret = write(fd, (void *)&pktlen, sizeof(pktlen));
	if(ret < 0) perror("write to ril failed");
	ret = write(fd, p->data, p->size);
	if(ret < 0) perror("write to ril failed");
}

void answer(void)
{
	struct parcel p;
	
	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_ANSWER);
	parcel_w_int32(&p, RIL_REQUEST_ANSWER);

	send_ril_parcel(&p);
	parcel_free(&p);
}

// power is 1 for on and 0 for off
void radio_power(int power)
{
	struct parcel p;

	parcel_init(&p);
	printf("setting radio power to %d\n", power);
	parcel_w_int32(&p, RIL_REQUEST_RADIO_POWER);
	parcel_w_int32(&p, power ? POWER_ON : POWER_OFF);
	parcel_w_int32(&p, 1);
	parcel_w_int32(&p, power);

	send_ril_parcel(&p);
	parcel_free(&p);
}

static char *tmpaudit(int pno, Bhdr *b)
{
	printf("tmpaudit: pno = %d, bhdr = %p ", pno, b);
	printf("magic = %x size = %d data = %p\n", b->magic, b->size, b->u.data);
	return NULL;
}

void dial(char *number)
{
	struct parcel p;
	struct mixer *mixer;
	struct mixer_ctl *ctl;

	af_setMode(MODE_IN_CALL);
	af_setParameters("routing=1");
	af_setVoiceVolume(1.0f);
	set_mute(0);

	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_DIAL);
	parcel_w_int32(&p, RIL_REQUEST_DIAL);
	parcel_w_string(&p, number);
	parcel_w_int32(&p, 0);
	parcel_w_int32(&p, 0);
	parcel_w_int32(&p, 0);

	send_ril_parcel(&p);
	parcel_free(&p);
}

void activate_net(void)
{
	struct parcel p;

	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_SETUP_DATA_CALL); // request ID
	parcel_w_int32(&p, RIL_REQUEST_SETUP_DATA_CALL); // seq number (?)
	parcel_w_int32(&p, 7); // undocumented magic.
	parcel_w_string(&p, "1"); // CDMA or GSM. 1 = GSM
	parcel_w_string(&p, "0"); // data profile. 0 = default
	parcel_w_string(&p, "wap.cingular"); // APN name
	parcel_w_string(&p, "WAP@CINGULARGPRS.COM"); // APN user
	parcel_w_string(&p, "cingular1"); // APN passwd
	parcel_w_string(&p, "0"); // auth type. 0 usually

	send_ril_parcel(&p);
	parcel_free(&p);
}

void deactivate_net(void)
{
	struct parcel p;
	
	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_DEACTIVATE_DATA_CALL);
	parcel_w_int32(&p, RIL_REQUEST_DEACTIVATE_DATA_CALL);
	parcel_w_string(&p, "1"); // FIXME: this should be the CID returned by
				  // RIL_REQUEST_SETUP_DATA_CALL response
	send_ril_parcel(&p);
	parcel_free(&p);
}

void send_sms(char *smsc_pdu, char *pdu)
{
	struct parcel p;

	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_SEND_SMS);
	parcel_w_int32(&p, RIL_REQUEST_SEND_SMS);
	parcel_w_int32(&p, 2);
	parcel_w_string(&p, smsc_pdu);
	parcel_w_string(&p, pdu);

	send_ril_parcel(&p);
	parcel_free(&p);
}

void acknowledge_sms(void)
{
	struct parcel p;

	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_SMS_ACKNOWLEDGE);
	parcel_w_int32(&p, RIL_REQUEST_SMS_ACKNOWLEDGE);
	parcel_w_int32(&p, 2);
	parcel_w_int32(&p, 1); // success
	parcel_w_int32(&p, 0); // failure cause if necessary

	send_ril_parcel(&p);
	parcel_free(&p);
}

void get_reg_state(void)
{
	struct parcel p;
	
	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_REGISTRATION_STATE);
	parcel_w_int32(&p, RIL_REQUEST_REGISTRATION_STATE);
	
	send_ril_parcel(&p);
	parcel_free(&p);
}

void set_mute(int muted)
{
	struct parcel p;
	
	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_SET_MUTE);
	parcel_w_int32(&p, RIL_REQUEST_SET_MUTE);
	parcel_w_int32(&p, 1);
	parcel_w_int32(&p, muted);

	send_ril_parcel(&p);
	parcel_free(&p);
}

void get_current_calls(void)
{
	struct parcel p;

	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_GET_CURRENT_CALLS);
	parcel_w_int32(&p, RIL_REQUEST_GET_CURRENT_CALLS);

	send_ril_parcel(&p);
	parcel_free(&p);
}

void hangup(int index)
{
	struct parcel p;
	
	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_HANGUP);
	parcel_w_int32(&p, RIL_REQUEST_HANGUP);
	parcel_w_int32(&p, 1);
	parcel_w_int32(&p, index);

	send_ril_parcel(&p);
	parcel_free(&p);
}
// tmp

struct mixer_ctl {
    struct mixer *mixer;
    struct snd_ctl_elem_info *info;
    char **ename;
};

struct mixer {
    int fd;
    struct snd_ctl_elem_info *info;
    struct mixer_ctl *ctl;
    unsigned count;
};


void auditmixer_close(struct mixer *mixer)
{
    unsigned n,m;

    if (mixer->fd >= 0)
	close(mixer->fd);

    if (mixer->ctl) {
	for (n = 0; n < mixer->count; n++) {
	    if (mixer->ctl[n].ename) {
		unsigned max = mixer->ctl[n].info->value.enumerated.items;
		for (m = 0; m < max; m++)
		    free(mixer->ctl[n].ename[m]);
		free(mixer->ctl[n].ename);
	    }
	}
	free(mixer->ctl);
    }

    if (mixer->info)
	free(mixer->info);

    free(mixer);
}

struct mixer *auditmixer_open(void)
{
    struct snd_ctl_elem_list elist;
    struct snd_ctl_elem_info tmp;
    struct snd_ctl_elem_id *eid = NULL;
    struct mixer *mixer = NULL;
    unsigned n, m;
    int fd;

    fd = open("/dev/snd/controlC0", O_RDWR);
    if (fd < 0)
	return 0;

    memset(&elist, 0, sizeof(elist));
    if (ioctl(fd, SNDRV_CTL_IOCTL_ELEM_LIST, &elist) < 0)
	goto fail;

    mixer = calloc(1, sizeof(*mixer));
    if (!mixer)
	goto fail;

    mixer->ctl = calloc(elist.count, sizeof(struct mixer_ctl));
    mixer->info = calloc(elist.count, sizeof(struct snd_ctl_elem_info));
    if (!mixer->ctl || !mixer->info)
	goto fail;

    eid = calloc(elist.count, sizeof(struct snd_ctl_elem_id));
    if (!eid)
	goto fail;

    mixer->count = elist.count;
    mixer->fd = fd;
    elist.space = mixer->count;
    elist.pids = eid;
    if (ioctl(fd, SNDRV_CTL_IOCTL_ELEM_LIST, &elist) < 0)
	goto fail;

    for (n = 0; n < mixer->count; n++) {
	struct snd_ctl_elem_info *ei = mixer->info + n;
	ei->id.numid = eid[n].numid;
	if (ioctl(fd, SNDRV_CTL_IOCTL_ELEM_INFO, ei) < 0)
	    goto fail;
	mixer->ctl[n].info = ei;
	mixer->ctl[n].mixer = mixer;
	if (ei->type == SNDRV_CTL_ELEM_TYPE_ENUMERATED) {
	    char **enames = calloc(ei->value.enumerated.items, sizeof(char*));
	    if (!enames)
		goto fail;
	    mixer->ctl[n].ename = enames;
	    for (m = 0; m < ei->value.enumerated.items; m++) {
		memset(&tmp, 0, sizeof(tmp));
		tmp.id.numid = ei->id.numid;
		tmp.value.enumerated.item = m;
		if (ioctl(fd, SNDRV_CTL_IOCTL_ELEM_INFO, &tmp) < 0)
		    goto fail;
		enames[m] = strdup(tmp.value.enumerated.name);
		if (!enames[m])
		    goto fail;
	    }
	}
    }

    free(eid);
    return mixer;

fail:
    if (eid)
	free(eid);
    if (mixer)
	mixer_close(mixer);
    else if (fd >= 0)
	close(fd);
    return 0;
}
