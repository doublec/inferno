#ifndef __PARCEL_H
#define __PARCEL_H

struct parcel {
	char *data;
	size_t offset;
	size_t capacity;
	size_t size;
};

int parcel_init(struct parcel *p);
int parcel_grow(struct parcel *p, size_t size);
void parcel_free(struct parcel *p);
int32_t parcel_r_int32(struct parcel *p);
int parcel_w_int32(struct parcel *p, int32_t val);
int parcel_w_string(struct parcel *p, char *str);
char *parcel_r_string(struct parcel *p);
size_t parcel_data_avail(struct parcel *p);

#endif
