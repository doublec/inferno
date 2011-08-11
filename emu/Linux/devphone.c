/* TODO/BUGS in no order:
   SMSes will be acknowledged regardless of whether anyone happens to be reading /phone/sms to see them, potentially losing SMSes if no one reads them before shutdown
   translation from UTF-8 to GSM for sending SMSes
   decode_sms is sloppy--should convert septets into octets, then convert octets into UTF-8 instead of trying to do it all at once. split off into separate functions?
   handle sending/receiving multiple text messages and checking length of messages sent to /phone/sms
   consider notifying RIL of screen state using RIL_REQUEST_SCREEN_STATE to conserve power
   figure out why default SMSC address for send_sms does not work
   too many assumptions made in phonewrite() about what the user sends
   too many assumptions made in loop_for_data() about read() giving exactly the right amount of data
   some of the created files will not be consistent across multiple read()s, i.e. if you read a few bytes of data, wait, then read a few more, the result may not make sense if things change in between. E.g. you may get "onf" if you read from /phone/ctl during a on/off transition
   parcel code will go crazy if ints overflow
   no way to give an error if SMSes don't send
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

#define RESPONSE_SOLICITED 0
#define RESPONSE_UNSOLICITED 1
#define POWER_ON 2000
#define POWER_OFF 2001

enum
{
	Qdir,
	Qctl,
	Qsms,
	Qphone,
	Qsignal,
	Qstatus
};

Dirtab phonetab[] =
{
	".",      {Qdir, 0, QTDIR}, 0, DMDIR|0555,
	"ctl",    {Qctl},           0, 0666,
	"sms",    {Qsms},           0, 0666,
	"phone",  {Qphone},         0, 0666,
	"signal", {Qsignal},        0, 0666,
	"status", {Qstatus},        0, 0666,
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

void loop_for_data(void *v);
void send_sms(char *smsc_pdu, char *pdu);
char *encode_sms(char *dest, char *msg);
void dial(char *number);
void activate_net(void);
void deactivate_net(void);
void radio_power(int power);

void phoneinit(void)
{
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
	if(seteuid(0) == -1) {
		perror("seteuid(0) failed");
	}
	if(setegid(0) == -1) {
		perror("setegid(0) failed");
	}
	kproc("phone", loop_for_data, 0, 0);
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
	}
	return 0;
}

static long phonewrite(Chan *c, void *va, long n, vlong offset)
{
	char *pdu;
	char *split;

	if(c->qid.type & QTDIR)
		error(Eperm);
	switch((ulong) c->qid.path) {
	case Qctl:
		((char *)va)[n] = '\0';
		printf("Qctl: va = %s\n", va);
		((char *) va)[strlen(va) - 1] = '\0'; // get rid of newline
		if(strcmp(va, "on") == 0) {
			radio_power(1);
		} else if(strcmp(va, "off") == 0) {
			radio_power(0);
		} else if(strcmp(va, "net on") == 0) {
			activate_net();
		} else if(strcmp(va, "net off") == 0) {
			deactivate_net();
		}
		break;
	case Qsms:
		printf("Qsms: got %d bytes\n", n);
		((char *)va)[n] = '\0';
		printf("Qsms: va = %s\n", va);
		((char *) va)[strlen(va) - 1] = '\0'; // get rid of newline
		split = strstr(va, ":");
		if(split == NULL) break;
		*split = '\0';
		split++;

		printf("dest = %s msg = %s\n", va, split);
		pdu = encode_sms(va, split);
		printf("pdu = %s\n", pdu);
		send_sms("07912180958729f4", pdu);
		free(pdu);
		break;
	case Qphone:
		((char  *)va)[n] = '\0';
		printf("Qphone: va = %s\n", va);
		((char *) va)[strlen(va) - 1] = '\0'; // get rid of newline
		if(strncmp(va, "answer", 6) == 0) {
			answer();
			return n;
		} else if(strncmp(va, "dial", 4)) {
			split = strstr(va, " ");
			if(split == NULL) break;
			*split = '\0';
			split++;
			
			dial(va);
		}
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
	p->data = malloc(sizeof(int32_t)); // arbitrary size to start with
	if(p->data == NULL) return -1;
	p->size = 0;
	p->capacity = sizeof(int32_t);
	p->offset = 0;
	return 0;
}

int parcel_grow(struct parcel *p, size_t size)
{
	char *new = realloc(p->data, p->capacity + size);

	if(new == NULL) {
		return -1;
	}
	p->data = new;
	p->capacity += size;
	return 0;
}

void parcel_free(struct parcel *p)
{
	free(p->data);
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
		/*printf("parcel_w_int32(%d): offset = %d, cap = %d, size = %d\n",
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
	// we have to avoid strdup8to16 here because inferno doesn't like us
	// free()ing the memory that strdup8to16 creates. 
	s16_len = strlen8to16(str);
	s16 = malloc(sizeof(char16_t) * s16_len);
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
				free(s16);
				return -1;
			}
		}
	}
	free(s16);
	return 0;
}

char *parcel_r_string(struct parcel *p)
{
	char *ret;
	int len = parcel_r_int32(p);
	ret = strndup16to8((char16_t *) (p->data + p->offset), len);
	p->offset += PAD_SIZE((len + 1) * sizeof(char16_t));
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
        *out = 'a'; printf("out = %02x\n", *out); return 1;
}

char *hexify(char *bytes, size_t len)
{
	char *hexarray[256];
	int i, j;
	char *ret;
	for(i = 0; i < 256; i++) {
		hexarray[i] = (char *) malloc(3 * sizeof(char));
		snprintf(hexarray[i], 3, "%02x", i);
	}
	ret = (char *) malloc((len * 2 + 1) * sizeof(char));
	for(i = 0, j = 0; i < len; i++, j += 2) {
		ret[j] = hexarray[(unsigned char) bytes[i]][0];
		ret[j + 1] = hexarray[(unsigned char) bytes[i]][1];
	}
	ret[j] = '\0';
	for(i = 0; i < 256; i++) {
		free(hexarray[i]);
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
	bytes = (char **) malloc((len / 2) * sizeof(char *));
	for(i = 0; i < len / 2; i++) {
		bytes[i] = (char *) malloc(3 * sizeof(char));
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
	service_center = (char *) malloc((smsc_len + 2) * sizeof(char));
	service_center[0] = '\0';
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
	src_num = (char *) malloc((src_len + 2) * sizeof(char));
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
	
	timestamp = (char *) malloc(15 * sizeof(char));
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
	// Calculate the # of UTF-8 bytes the message will take up by
	// doing a first pass with gsm_to_ascii - this is terrible and could 
	// probably be done in a better way. FIXME
/*	oldbits = 0;
	numoldbits = 0;
	for(i = 0, j = 0; i < msg_len; i++) {
		int numbits = (i % 7) + 1;
		int newbits;
		char garbage[4];
		newbits = msgbytes[i] >> (8 - numbits);
		newbits &= (1 << numbits) - 1;
		msgbytes[i] = msgbytes[i] & ((1 << (8 - numbits)) - 1);
		msgbytes[i] = msgbytes[i] << numoldbits;
		// repurpose gsm_to_utf8 to just give us the length. bad
		utf8msglen += gsm_to_utf8(((msgbytes[i] & ((1 << (8 - numbits)) - 1)) << numoldbits) | oldbits, garbage);
		if(numbits == 7) {
			utf8msglen += gsm_to_utf8(newbits, garbage);
			newbits = 0;
			numbits = 0;
		}
		oldbits = newbits;
		numoldbits = numbits;
	}
	printf("calculated utf8msglen = %d\n", utf8msglen);
*/	utf8msglen = 400;
	msg = (char *) malloc((utf8msglen + 1) * sizeof(char));

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
		free(bytes[i]);
	}
	free(bytes);
	return 0;
}

char *encode_sms(char *destnum, char *msg)
{
	int i, j, actual_dest_len, enc_msg_len, ret_len;
	char dest_len[3];
	char *dest;
	char *enc_msg, *final_msg, *ret;
	// dest_len is the hex representation of the length of the phone #
	snprintf(dest_len, 3, "%02x", strlen(destnum));
	actual_dest_len = (strlen(destnum) % 2 == 0) ? strlen(destnum) : strlen(destnum) + 1;
	dest = (char *) malloc((actual_dest_len + 1) * sizeof(char));
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
	enc_msg_len = strlen(msg) - (strlen(msg) / 8);
	enc_msg = (char *) malloc((enc_msg_len + 1) * sizeof(char));
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
//		printf("%hhx\n", enc_msg[j]);
		fflush(stdout);
	}
	final_msg = hexify(enc_msg, enc_msg_len);
	ret_len = 14 + strlen(dest) + strlen(final_msg);
	ret = (char *) malloc((ret_len + 1) * sizeof(char));
	// FIXME: too much of the sent msg is hardcoded
	snprintf(ret, ret_len + 1, "0120%s91%s0000%02x%s", dest_len, dest, strlen(msg), final_msg);
//	printf("%s\n", ret);
	free(enc_msg);
	free(final_msg);
	free(dest);
	return ret;
}

/* functions that actually belong here */

void interpret_sol_response(struct parcel *p)
{
	int seq, error, i, numstr, offset = 0;
	char buf[200];
	seq = parcel_r_int32(p);
	error = parcel_r_int32(p);
	printf("solicited: seq = %d error = %d\n", seq, error);
	printf("%d bytes left\n", parcel_data_avail(p));
	if(error != 0) {
		return;
	}
	switch(seq) {
	case RIL_REQUEST_SETUP_DATA_CALL:
		if(!parcel_data_avail(p)) return;
		numstr = parcel_r_int32(p);
		printf("numstr = %d\n", numstr);
		for(i = 0; i < numstr; i++) {
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
		numstr = parcel_r_int32(p);
		offset = 0;
		for(i = 0; i < numstr; i++) {
			offset += snprintf(buf + offset, sizeof(buf) - offset, "%s\n", parcel_r_string(p));
		}
		// FIXME lock
		free(status_msg.msg);
		status_msg.msg = strdup(buf);
		sem_post(&status_msg.sem);
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

		free(sms.msg);
		free(sms.service_center);
		free(sms.src_num);
		free(sms.timestamp);
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
		void *pdata;

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
		/*	for(i = 0; i < readlen; i++) {
			printf("%02x", buf[i]);
		}
		printf("\n");*/
		parcel_grow(&p, msglen);
		memcpy(p.data, buf, msglen);
		p.size = msglen;
		//	printf("Received %d bytes of data from rild.\n", msglen);
		type = parcel_r_int32(&p);
		if(type == RESPONSE_SOLICITED) {
			interpret_sol_response(&p);
		} else {
			interpret_unsol_response(&p);
		}
		free(buf);
		sleep(2); // FIXME may not be necessary
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

void dial(char *number)
{
	struct parcel p;
	
	parcel_init(&p);
	parcel_w_int32(&p, RIL_REQUEST_DIAL);
	parcel_w_int32(&p, RIL_REQUEST_DIAL);
	parcel_w_string(&p, number);
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
