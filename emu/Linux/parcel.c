/* Parcel-handling code */
#include <sys/types.h>
#include <cutils/jstring.h>
#include <string.h>
#include "audit.h"
#include <endian.h>

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
