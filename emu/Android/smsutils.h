#ifndef __SMSUTILS_H
#define __SMSUTILS_H
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

int decode_sms(char *hexstr, struct recvd_sms *sms);
char *encode_sms(char *destnum, Rune *runemsg);
#endif
