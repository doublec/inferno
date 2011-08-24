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
// tmp
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
char *encode_sms(char *destnum, Rune *runemsg);
void dial(char *number);
void activate_net(void);
void deactivate_net(void);
void radio_power(int power);
int rune_to_gsm(Rune r, char *out);

void *auditmalloc(size_t orig);
void auditfree(void *p);
void auditmixer_close(struct mixer *mixer);
struct mixer *auditmixer_open(void);
void *auditcalloc(size_t nmemb, size_t size);
char *auditstrdup(char *str);
void *auditrealloc(void *p, size_t size);

void phoneinit(void)
{
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
	ril_client_handle = dlopen("libsecril-client.so", RTLD_NOW);
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
	}
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

/* Parcel-handling code */
#define PAD_SIZE(s) (((s)+3)&~3)

struct parcel {
	char *data;
	size_t offset;
	size_t capacity;
	size_t size;
};

int parcel_init(struct parcel *p)
{
	p->data = auditmalloc(sizeof(int32_t)); // arbitrary size to start with
	if(p->data == NULL) return -1;
	p->size = 0;
	p->capacity = sizeof(int32_t);
	p->offset = 0;
	return 0;
}

int parcel_grow(struct parcel *p, size_t size)
{
	char *new = auditrealloc(p->data, p->capacity + size);

	if(new == NULL) {
		return -1;
	}
	p->data = new;
	p->capacity += size;
	return 0;
}

void parcel_free(struct parcel *p)
{
	auditfree(p->data);
	p->size = 0;
	p->capacity = 0;
	p->offset = 0;
}

int32_t parcel_r_int32(struct parcel *p)
{
	int32_t ret;
	ret = *((int32_t *) (p->data + p->offset));
	p->offset += sizeof(int32_t);
	return ret;
}

int parcel_w_int32(struct parcel *p, int32_t val)
{
	for(;;) {
/*		printf("parcel_w_int32(%d): offset = %d, cap = %d, size = %d\n",
		val, p->offset, p->capacity, p->size);*/
		if(p->offset + sizeof(int32_t) < p->capacity) {
			// There's enough space
			*((int32_t *) (p->data + p->offset)) = val;
			p->offset += sizeof(int32_t);
			p->size += sizeof(int32_t);
			break;
		} else {
			// Grow data and retry
			if(parcel_grow(p, sizeof(int32_t)) == -1) {
				return -1;
			}
		}
	}
	return 0;
}

int parcel_w_string(struct parcel *p, char *str)
{
	char16_t *s16;
	size_t s16_len;
	size_t len;

	if(str == NULL) {
		parcel_w_int32(p, -1);
		return 0;
	}
	s16_len = strlen8to16(str);
	s16 = auditmalloc(sizeof(char16_t) * s16_len);
	strcpy8to16(s16, str, &s16_len);
	if(parcel_w_int32(p, s16_len) == -1) {
		return -1;
	}
	len = (s16_len + 1) * sizeof(char16_t);
	for(;;) {
		size_t padded = PAD_SIZE(len);
		/*printf("parcel_w_string(\"%s\"): offset %d, cap %d, size %d\n",
		  str, p->offset, p->capacity, p->size);*/
		if(p->offset + len < p->capacity) {
			// There's enough space
			memcpy(p->data + p->offset, s16, s16_len * sizeof(char16_t));
			*((char16_t *) (p->data + p->offset + len)) = 0;
			p->offset += padded;
			p->size += padded;
			if (padded != len) {
				//printf("Writing %ld bytes, padded to %ld\n", len, padded);
#if BYTE_ORDER == BIG_ENDIAN
				static const uint32_t mask[4] = {
					0x00000000, 0xffffff00, 0xffff0000, 0xff000000
				};
#endif
#if BYTE_ORDER == LITTLE_ENDIAN
				static const uint32_t mask[4] = {
					0x00000000, 0x00ffffff, 0x0000ffff, 0x000000ff
				};
#endif
				//printf("Applying pad mask: %p to %p\n", (void*)mask[padded-len],
				//    *reinterpret_cast<void**>(data+padded-4));
				*((uint32_t*)(p->data+p->offset+padded-4)) &= mask[padded-len];
			}
			break;
		} else {
			// Grow data and retry
			if(parcel_grow(p, padded) == -1) {
				auditfree(s16);
				return -1;
			}
		}
	}
	auditfree(s16);
	return 0;
}

char *parcel_r_string(struct parcel *p)
{
	char *ret;
	int len16 = parcel_r_int32(p);
	size_t len8;
	if(len16 < 0) return NULL; // this is how a null string is sent
	len8 = strnlen16to8((char16_t *) (p->data + p->offset), len16);
	ret = auditmalloc(len8 + 1);
	if(ret == NULL) return NULL;
	strncpy16to8(ret, (char16_t *) (p->data + p->offset), len16);
	p->offset += PAD_SIZE((len16 + 1) * sizeof(char16_t));
	return ret;
}

size_t parcel_data_avail(struct parcel *p)
{
	return (p->size - p->offset);
}

/* Received SMS - we get a hex string representation of the data.
   Roughly, the layout of the data is:
   1 byte - length of SMS center information
   1 byte - the format of the number (91 is the standard)
   (length above - 1) bytes - the number, nibble-swapped and padded with an
   f if needed
   1 byte - properties of the message, generally we can ignore this
   1 byte - length of sender number (in digits, not bytes)
   variable bytes - sender number, nibble-swapped and padded if needed
   1 byte - protocol identifier - can ignore this probably
   1 byte - data coding scheme - again can hopefully be ignored
   7 bytes - timestamp, again nibble-swapped: y, m, d, h, m, s, timezone
   * timezone is in units of +15 min vs. GMT. If most sig. bit is
   1, value is in units of -15 min vs. GMT.
   1 byte - length of message in characters (NOT in bytes)
   rest of bytes - message. if ASCII 7-bit, compacted to save space through
   an encoding mechanism.
   For more information see http://www.dreamfabric.com/sms/ or GSM 03.38
*/

struct recvd_sms {
	char *src_num;
	char *service_center;
	char *msg;
	char *timestamp;
};

char gsm_to_ascii(char gsm);
int decode_sms(char *hexstr, struct recvd_sms *sms);
void acknowledge_sms(void);

void usage(char *name);


/* SMS-related functions */

// writes UTF-8 translation to out and returns number of bytes written
int gsm_to_utf8(char gsm, char *out)
{
	switch(gsm) {
	case '\0': *out = '@'; return 1;
	case 1: out[0] = 0xC2; out[1] = 0xA3; return 2; // £
	case 2: *out = '$'; return 1;
	case 3: out[0] = 0xC2; out[1] = 0xA5; return 2; // ¥
	case 4: out[0] = 0xC3; out[1] = 0xA8; return 2; // è
	case 5: out[0] = 0xC3; out[1] = 0xA9; return 2; // é
	case 6: out[0] = 0xC3; out[1] = 0xB9; return 2; // ù
	case 7: out[0] = 0xC3; out[1] = 0xAC; return 2; // ì
	case 8: out[0] = 0xC3; out[1] = 0xB2; return 2; // ò
	case 9: out[0] = 0xC3; out[1] = 0x87; return 2; // Ç
	case 11: out[0] = 0xC3; out[1] = 0x98; return 2; // Ø
	case 12: out[0] = 0xC3; out[1] = 0xB8; return 2; // ø
	case 14: out[0] = 0xC3; out[1] = 0x85; return 2; // Å
	case 15: out[0] = 0xC3; out[1] = 0xA5; return 2; // å
	case 16: out[0] = 0xCE; out[1] = 0x94; return 2; // Δ
	case 17: *out = '_'; return 1;
	case 18: out[0] = 0xCE; out[1] = 0xA6; return 2; // Φ
	case 19: out[0] = 0xCE; out[1] = 0x93; return 2; // Γ
	case 20: out[0] = 0xCE; out[1] = 0x9B; return 2; // Λ
	case 21: out[0] = 0xCE; out[1] = 0xA9; return 2; // Ω
	case 22: out[0] = 0xCE; out[1] = 0xA0; return 2; // Π
	case 23: out[0] = 0xCE; out[1] = 0xA8; return 2; // Ψ
	case 24: out[0] = 0xCE; out[1] = 0xA3; return 2; // Σ
	case 25: out[0] = 0xCE; out[1] = 0x98; return 2; // Θ
	case 26: out[0] = 0xCE; out[1] = 0x9E; return 2; // Ξ
	case 27: // extension character
		*out = '?'; return 1;
	case 28: out[0] = 0xC3; out[1] = 0x86; return 2; // Æ
	case 29: out[0] = 0xC3; out[1] = 0xA6; return 2; // æ
	case 30: out[0] = 0xC3; out[1] = 0x9F; return 2; // ß
	case 31: out[0] = 0xC3; out[1] = 0x89; return 2; // É
	case 36: out[0] = 0xC2; out[1] = 0xA4; return 2; // ¤
	case 64: out[0] = 0xC2; out[1] = 0xA1; return 2; // ¡
	case 91: out[0] = 0xC3; out[1] = 0x84; return 2; // Ä
	case 92: out[0] = 0xC3; out[1] = 0x96; return 2; // Ö
	case 93: out[0] = 0xC3; out[1] = 0x91; return 2; // Ñ
	case 94: out[0] = 0xC3; out[1] = 0x9C; return 2; // Ü
	case 95: out[0] = 0xC2; out[1] = 0xA7; return 2; // §
	case 96: out[0] = 0xC2; out[1] = 0xBF; return 2; // ¿
	case 123: out[0] = 0xC3; out[1] = 0xA4; return 2; // ä
	case 124: out[0] = 0xC3; out[1] = 0xB6; return 2; // ö
	case 125: out[0] = 0xC3; out[1] = 0xB1; return 2; // ñ
	case 126: out[0] = 0xC3; out[1] = 0xBC; return 2; // ü
	case 127: out[0] = 0xC3; out[1] = 0xA0; return 2; // à
	default: *out = gsm; return 1;
	}
}

// Unicode to GSM SMS format, returns number of bytes written to out
int rune_to_gsm(Rune r, char *out)
{
	switch(r) {
	case L'@': *out = 0; return 1;
	case L'£': *out = 1; return 1;
	case L'$': *out = 2; return 1;
	case L'¥': *out = 3; return 1;
	case L'è': *out = 4; return 1;
	case L'é': *out = 5; return 1;
	case L'ù': *out = 6; return 1;
	case L'ì': *out = 7; return 1;
	case L'ò': *out = 8; return 1;
	case L'Ç': *out = 9; return 1;
	case L'Ø': *out = 11; return 1;
	case L'ø': *out = 12; return 1;
	case L'Å': *out = 14; return 1;
	case L'å': *out = 15; return 1;
	case L'Δ': *out = 16; return 1;
	case L'_': *out = 17; return 1;
	case L'Φ': *out = 18; return 1;
	case L'Γ': *out = 19; return 1;
	case L'Λ': *out = 20; return 1;
	case L'Ω': *out = 21; return 1;
	case L'Π': *out = 22; return 1;
	case L'Ψ': *out = 23; return 1;
	case L'Σ': *out = 24; return 1;
	case L'Θ': *out = 25; return 1;
	case L'Ξ': *out = 26; return 1;
	case L'\f': out[0] = 27; out[1] = 10; return 2;
	case L'^': out[0] = 27; out[1] = 20; return 2;
	case L'{': out[0] = 27; out[1] = 40; return 2;
	case L'}': out[0] = 27; out[1] = 41; return 2;
	case L'\\': out[0] = 27; out[1] = 47; return 2;
	case L'[': out[0] = 27; out[1] = 60; return 2;
	case L'~': out[0] = 27; out[1] = 61; return 2;
	case L']': out[0] = 27; out[1] = 62; return 2;
	case L'|': out[0] = 27; out[1] = 64; return 2;
	case L'€': out[0] = 27; out[1] = 101; return 2;
	case L'Æ': *out = 28; return 1;
	case L'æ': *out = 29; return 1;
	case L'ß': *out = 30; return 1;
	case L'É': *out = 31; return 1;
	case L'¤': *out = 36; return 1;
	case L'¡': *out = 64; return 1;
	case L'Ä': *out = 91; return 1;
	case L'Ö': *out = 92; return 1;
	case L'Ñ': *out = 93; return 1;
	case L'Ü': *out = 94; return 1;
	case L'§': *out = 95; return 1;
	case L'¿': *out = 96; return 1;
	case L'ä': *out = 123; return 1;
	case L'ö': *out = 124; return 1;
	case L'ñ': *out = 125; return 1;
	case L'ü': *out = 126; return 1;
	case L'à': *out = 127; return 1;
	default: *out = r; return 1;
	}
}

char *hexify(char *bytes, size_t len)
{
	char *hexarray[256];
	int i, j;
	char *ret;
	for(i = 0; i < 256; i++) {
		hexarray[i] = (char *) auditmalloc(3 * sizeof(char));
		snprintf(hexarray[i], 3, "%02x", i);
	}
	ret = (char *) auditmalloc((len * 2 + 1) * sizeof(char));
	for(i = 0, j = 0; i < len; i++, j += 2) {
		ret[j] = hexarray[(unsigned char) bytes[i]][0];
		ret[j + 1] = hexarray[(unsigned char) bytes[i]][1];
	}
	ret[j] = '\0';
	for(i = 0; i < 256; i++) {
		auditfree(hexarray[i]);
	}
	return ret;
}

int decode_sms(char *hexstr, struct recvd_sms *sms)
{
	int smsc_len, src_len, curpos, msg_len;
	char *endptr;
	char **bytes;
	char *service_center, *src_num, *timestamp, *msg;
	int len, i, j, oldbits, numoldbits, utf8msglen = 0;
	char msgbytes[160];
    
	/* first, split the string up into hex representations of the bytes--
	   this makes things much easier overall */
	len = strlen(hexstr);
	if(len % 2 != 0) {
		fprintf(stderr, "malformed hex string passed to decode_sms\n");
		return -1;
	}
	bytes = (char **) auditmalloc((len / 2) * sizeof(char *));
	for(i = 0; i < len / 2; i++) {
		bytes[i] = (char *) auditmalloc(3 * sizeof(char));
		bytes[i][0] = hexstr[i * 2];
		bytes[i][1] = hexstr[i * 2 + 1];
		bytes[i][2] = '\0';
	}

	// decode the service center number
	smsc_len = strtol(bytes[0], &endptr, 16);
	if(strcmp(bytes[1], "91")) {
		fprintf(stderr, "unsupported number format for sender: %s",
			bytes[1]);
	}
	service_center = auditmalloc((smsc_len*2 + 2) * sizeof(char));
	for(i = 2, j = 0; i < smsc_len + 1; i++, j += 2) {
		// number is nibble-swapped
		service_center[j] = bytes[i][1];
		if(bytes[i][0] == 'f') { // num may be padded with 'f' at end
			service_center[j + 1] = '\0';
		} else {
			service_center[j + 1] = bytes[i][0];
		}
	}
	service_center[j] = '\0';
	curpos = i;
	sms->service_center = service_center;

	// next, decode the sender's number
	src_len = strtol(bytes[curpos + 1], NULL, 16);
	src_num = (char *) auditmalloc((src_len + 2) * sizeof(char));
	// convert src_len from # of digits to # of bytes
	src_len = (src_len % 2 == 0) ? src_len : src_len + 1;
	src_len = src_len / 2;
	if(strcmp(bytes[curpos + 2], "91")) {
		fprintf(stderr, "unsupported number format for sender: %s",
			bytes[curpos + 2]);
	}
	for(i = curpos + 3, j = 0; i < curpos + 3 + src_len; i++, j += 2) {
		// number is nibble-swapped
		src_num[j] = bytes[i][1];
		if(bytes[i][0] == 'f') { // num may be padded with 'f' at end
			src_num[j + 1] = '\0';
		} else {
			src_num[j + 1] = bytes[i][0];
		}
	}
	src_num[j] = '\0';
	sms->src_num = src_num;
	curpos = i;

	// Timestamp
	// TODO: may want to convert this to a time_t value as well
	
	timestamp = (char *) auditmalloc(15 * sizeof(char));
	for(i = curpos + 2, j = 0; i < curpos + 9; i++, j += 2) {
		// number is nibble-swapped
		timestamp[j] = bytes[i][1];
		timestamp[j + 1] = bytes[i][0];
	}
	curpos = i;
	timestamp[j] = '\0';
	sms->timestamp = timestamp;

	// Message

	msg_len = (len / 2) - (curpos + 1);
	for(i = curpos + 1, j = 0; i < len / 2, j < msg_len; i++, j++) {
		msgbytes[j] = strtol(bytes[i], NULL, 16);
	}
	// FIXME?
	utf8msglen = UTFmax*161;
	msg = (char *) auditmalloc((utf8msglen + 1) * sizeof(char));

	// convert message to UTF-8
	oldbits = 0;
	numoldbits = 0;
	for(i = 0, j = 0; i < msg_len; i++) {
		int numbits = (i % 7) + 1;
		int newbits;
		newbits = msgbytes[i] >> (8 - numbits);
		newbits &= (1 << numbits) - 1;
		msgbytes[i] = msgbytes[i] & ((1 << (8 - numbits)) - 1);
		msgbytes[i] = msgbytes[i] << numoldbits;
		// GSM alphabet is different than UTF-8 alphabet
		j += gsm_to_utf8(msgbytes[i] | oldbits, msg + j);
		if(numbits == 7) {
			msg[j] = newbits;
			j++;
			newbits = 0;
			numbits = 0;
		}
		oldbits = newbits;
		numoldbits = numbits;
	}
	msg[j] = '\0';
	sms->msg = msg;
	// cleanup
	for(i = 0; i < len / 2; i++) {
		auditfree(bytes[i]);
	}
	auditfree(bytes);
	return 0;
}

char *encode_sms(char *destnum, Rune *runemsg)
{
	int i, j, actual_dest_len, enc_msg_len, ret_len;
	char dest_len[3];
	char *dest;
	char *enc_msg, *final_msg, *ret, *msg;
	// dest_len is the hex representation of the length of the phone #
	snprintf(dest_len, 3, "%02x", strlen(destnum));
	actual_dest_len = (strlen(destnum) % 2 == 0) ? strlen(destnum) : strlen(destnum) + 1;
	dest = (char *) auditmalloc((actual_dest_len + 1) * sizeof(char));
	for(i = 0; i < actual_dest_len; i += 2) {
		if(destnum[i + 1] != '\0') {
			dest[i] = destnum[i + 1];
		} else {
			dest[i] = 'f';
		}
		dest[i + 1] = destnum[i];
	}
	dest[i] = '\0';
//	printf("dest = %s\n", dest);

	msg = auditmalloc(UTFmax * (runestrlen(runemsg) + 1) * sizeof(char));
	for(i = 0, j = 0; i < runestrlen(runemsg); i++) {
		j += rune_to_gsm(runemsg[i], msg + j);
	}
	msg[j] = '\0';
	for(i = 0; i < strlen(msg); i++) {
		printf("%02x\n", msg[i]);
	}
	enc_msg_len = strlen(msg) - (strlen(msg) / 8);
	enc_msg = (char *) auditmalloc((enc_msg_len + 1) * sizeof(char));
	for(i = 0, j = 0; i < strlen(msg); i++, j++) {
		int numbits = (i + 1) % 8;
		char bits;
		if(numbits == 0) {
			j--;
			continue;
		}
		bits = msg[i + 1] & ((1 << numbits) - 1);
		enc_msg[j] = msg[i] >> numbits - 1;
//			printf("once shifted by %d: %hhx\n", numbits, enc_msg[j]);
		enc_msg[j] = enc_msg[j] | (bits << (8 - numbits));
//			printf("bits << (8 - numbits) %hhx\n", bits << (8 - numbits));
	}
	final_msg = hexify(enc_msg, enc_msg_len);
	ret_len = 14 + strlen(dest) + strlen(final_msg);
	ret = (char *) auditmalloc((ret_len + 1) * sizeof(char));
	// FIXME: too much of the sent msg is hardcoded
	snprintf(ret, ret_len + 1, "0120%s91%s0000%02x%s", dest_len, dest, strlen(msg), final_msg);
	auditfree(enc_msg);
	auditfree(final_msg);
	auditfree(dest);
	return ret;
}

/* functions that actually belong here */

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
//		setCallClockSync(ril_client, SOUND_CLOCK_START);
		printf("setcallvolume returned %d\n",
		       setCallVolume(ril_client, SOUND_TYPE_VOICE, 4));
		printf("setcallaudiopath returned %d\n", setCallAudioPath(ril_client, SOUND_AUDIO_PATH_HANDSET));
		printf("setcallvolume returned %d\n",
		       setCallVolume(ril_client, SOUND_TYPE_VOICE, 4));
		printf("got RIL_REQUEST_DIAL success\n");
		set_mute(0);
//		setCallClockSync(ril_client, SOUND_CLOCK_START);
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
	Bhdr *b; // tmp

	setCallClockSync(ril_client, SOUND_CLOCK_START);

	set_mute(0);

	printf("isConnectedRILD = %d\n", isConnectedRILD(ril_client));

	mixer = auditmixer_open();

	if(mixer == NULL) {
		fprintf(stderr, "failed to open mixer\n");
	}
	//printf("==============\n");
	//poolaudit(tmpaudit);
	//printf("mixer = %p sizeof(ulong) = %d sizeof(mixer) = %d\n", mixer, sizeof(ulong), sizeof(mixer));
	//b = ((Bhdr*)(((uchar*)mixer)-(((Bhdr*)0)->u.data)));
	//printf("header at %p; magic: %x\n", b, b->magic);
	//D2B(b, mixer); // tmp
	
	ctl = mixer_get_control(mixer, "Input Source", 0);
	mixer_ctl_select(ctl, "Default");

	ctl = mixer_get_control(mixer, "Voice Call Path", 0);
	mixer_ctl_select(ctl, "RCV");
	setCallClockSync(ril_client, SOUND_CLOCK_START);
	AUDITCHECK;

	printf("setcallvolume returned %d\n",
	       setCallVolume(ril_client, SOUND_TYPE_SPEAKER, 4));
	printf("setcallaudiopath returned %d\n", setCallAudioPath(ril_client, SOUND_AUDIO_PATH_HANDSET));
	printf("setcallvolume returned %d\n",
	       setCallVolume(ril_client, SOUND_TYPE_VOICE, 4));


	printf("before mixer_close\n");
	fflush(stdout);
	auditmixer_close(mixer);
	printf("after mixer_close");
	fflush(stdout);
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

// memory audit stuff
struct alloc_node {
	void *block;
	struct alloc_node *next;
};

#define AUDITCHECK printf("started auditcheck at %s:%d\n", __FILE__, __LINE__); \
	auditcheck();							\
	printf("finished auditcheck\n");

struct alloc_node *alloc_head = NULL;

void add_alloc_node(void *block)
{
	struct alloc_node *node = malloc(sizeof(struct alloc_node));
	struct alloc_node *cur;

	node->block = block;
	node->next = NULL;
	if(alloc_head == NULL) {
		alloc_head = node;
	} else {
		for(cur = alloc_head; ; cur = cur->next) {
			if(cur->next == NULL) {
				cur->next = node;
				break;
			}
		}
	}
}

void del_alloc_node(void *block)
{
	struct alloc_node *cur;
	struct alloc_node *prev;

	for(cur = alloc_head; cur != nil; cur = cur->next) {
		if(cur->block == block) {
			if(cur == alloc_head) {
				alloc_head = cur->next;
			} else {
				prev->next = cur->next;
			}
			free(cur);
			return;
		}
		prev = cur;
	}
	fprintf(stderr, "del_alloc_node: could not find block %p\n", block);
}

void auditcheck(void)
{
	struct alloc_node *cur;
	for(cur = alloc_head; cur != NULL; cur = cur->next) {
		void *p = cur->block;
		long hdr, canary;
		size_t orig;
		
		hdr = *((long *)p - 2);
		if(hdr != 0xcafeface) {
			fprintf(stderr, "auditcheck: bad header at block %p, found %x\n",
				p, canary);
		}
		orig = *((size_t *) p - 1);
		canary = *((long *)(p + orig));
		if(canary != 0xaaaaaaaa) {
			fprintf(stderr, "auditcheck: bad canary at block %p, found %x\n",
				p, canary);
		}
	}
}

void *auditmalloc(size_t orig)
{
	size_t size = orig + sizeof(size_t) + 2*sizeof(long);
	void *ret;
	ret = malloc(size);
	if(ret == NULL) return NULL;
	*((long *) ret) = 0xcafeface;
	*(((size_t *) ret) + 1) = orig;
	*((long *) ((char *)ret + orig + sizeof(long) + sizeof(size_t))) = 0xaaaaaaaa;
	ret += sizeof(long) + sizeof(size_t);
	add_alloc_node(ret);
	return ret; 
}

void auditfree(void *p)
{
	long hdr, canary;
	size_t orig;

	if(p == NULL) return;
	
	hdr = *((long *)p - 2);
	if(hdr != 0xcafeface) {
		fprintf(stderr, "auditfree: bad header at block %p, found %x\n",
			p, canary);
	}
	orig = *((size_t *) p - 1);
	canary = *((long *)(p + orig));
	if(canary != 0xaaaaaaaa) {
		fprintf(stderr, "auditfree: bad canary at block %p, found %x\n",
			p, canary);
	}
	del_alloc_node(p);
	p = p - sizeof(long) - sizeof(size_t);
	memset(p, 0xbadbad, orig + 2 * sizeof(long) + sizeof(size_t));
	free(p);
}

char *auditstrdup(char *str)
{
	char *ret;
	ret = auditmalloc((strlen(str) + 1) * sizeof(char));
	strcpy(ret, str);
	return ret;
}

void *auditcalloc(size_t nmemb, size_t size)
{
	size_t total = nmemb * size;
	void *ret;
	ret = auditmalloc(total);
	memset(ret, 0, total);
	return ret;
}


void *auditrealloc(void *p, size_t size)
{
	void *ret;
	size_t orig = size;
	del_alloc_node(p);
	p = p - sizeof(size_t) - sizeof(long);
	size += 2 * sizeof(long) + sizeof(size_t);
	ret = realloc(p, size);
	*(((size_t *) ret) + 1) = orig;

	ret += sizeof(size_t) + sizeof(long);
	*((long *)(ret + orig)) = 0xaaaaaaaa;
	add_alloc_node(ret);
	return ret;
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
		    auditfree(mixer->ctl[n].ename[m]);
		auditfree(mixer->ctl[n].ename);
	    }
	}
	auditfree(mixer->ctl);
    }

    if (mixer->info)
	auditfree(mixer->info);

    auditfree(mixer);
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

    mixer = auditcalloc(1, sizeof(*mixer));
    if (!mixer)
	goto fail;

    mixer->ctl = auditcalloc(elist.count, sizeof(struct mixer_ctl));
    mixer->info = auditcalloc(elist.count, sizeof(struct snd_ctl_elem_info));
    if (!mixer->ctl || !mixer->info)
	goto fail;

    eid = auditcalloc(elist.count, sizeof(struct snd_ctl_elem_id));
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
	    char **enames = auditcalloc(ei->value.enumerated.items, sizeof(char*));
	    if (!enames)
		goto fail;
	    mixer->ctl[n].ename = enames;
	    for (m = 0; m < ei->value.enumerated.items; m++) {
		memset(&tmp, 0, sizeof(tmp));
		tmp.id.numid = ei->id.numid;
		tmp.value.enumerated.item = m;
		if (ioctl(fd, SNDRV_CTL_IOCTL_ELEM_INFO, &tmp) < 0)
		    goto fail;
		enames[m] = auditstrdup(tmp.value.enumerated.name);
		if (!enames[m])
		    goto fail;
	    }
	}
    }

    auditfree(eid);
    return mixer;

fail:
    if (eid)
	auditfree(eid);
    if (mixer)
	mixer_close(mixer);
    else if (fd >= 0)
	close(fd);
    return 0;
}
