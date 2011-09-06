#include "dat.h"
#include "fns.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "smsutils.h"
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
	case 27: // TODO extension character
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
	service_center = malloc((smsc_len*2 + 2) * sizeof(char));
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
	// FIXME?
	utf8msglen = UTFmax*161;
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

char *encode_sms(char *destnum, Rune *runemsg)
{
	int i, j, actual_dest_len, enc_msg_len, ret_len, msg_len;
	char dest_len[3];
	char *dest;
	char *enc_msg, *final_msg, *ret, *msg;
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

	msg = malloc(UTFmax * (runestrlen(runemsg) + 1) * sizeof(char));
	for(i = 0, j = 0; i < runestrlen(runemsg); i++) {
		j += rune_to_gsm(runemsg[i], msg + j);
	}
	msg[j] = '\0';
	msg_len = j;

	enc_msg_len = msg_len - (msg_len / 8);
	enc_msg = (char *) malloc((enc_msg_len + 1) * sizeof(char));
	for(i = 0, j = 0; i < msg_len; i++, j++) {
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
	ret = (char *) malloc((ret_len + 1) * sizeof(char));
	// FIXME: too much of the sent msg is hardcoded
	snprintf(ret, ret_len + 1, "0120%s91%s0000%02x%s", dest_len, dest, msg_len, final_msg);
	free(enc_msg);
	free(final_msg);
	free(dest);
	return ret;
}
