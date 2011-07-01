/*
 * This file is based on a port to the OLPC by Andrey Mirtchovski et. al.
 */

#include "dat.h"
#include "fns.h"
#include "../port/error.h"
#include "framebuffer.h"
#include "kernel.h"
#include <linux/input.h>

#include <draw.h>
#include <memdraw.h>
#include <cursor.h>

Point mousexy(void);

static ulong displaychannel = 0;
static int screeninited = 0;
static unsigned char* screendata = 0;
static unsigned char* framebuffer = 0;
static Point pointerposition = { 0, 0 };
static Lock pointerlock;
static uchar* pointer = 0;
static int pointerheight = 0;
static int pointerwidth = 0;

static void initscreen ( int, int, ulong*, int* );
void drawpointer ( int, int );

static int eventfd;
static int mousepid;

static int b = 0;
static int DblTime = 300000;
static int x, y, p, dbl = 0;
static int lastb;
static int touch = 0;
static int lastval = 0;

static void 
touchscreen(struct input_event* ev, int count)
{
	int i;
	static struct timeval lastt;

	for (i=0; i < count; i++){
		if(0) fprint(2, "%d/%d [%d] ", i, count, ev[i].code);
		switch(ev[i].type){
		case EV_ABS:
			switch(ev[i].code){
			case ABS_X:
				x = ev[i].value;
				break;
			case ABS_Y:
				y = ev[i].value;
				break;
			case ABS_PRESSURE:
				p = ev[i].value;
				break;
			case 0x30:		// ABS_MT_TOUCH_MAJOR
				if (ev[i].value /* && lastval == 0*/) {
					touch = 1;
					b = 1;
				} else if (ev[i].value == 0 /*&& lastval == 0*/) {
					touch = 0;
					b = 0;
				}
				//lastval = ev[i].value;
			case 0x36:		// ABS_MT_POSITION_X
				//if (touch && lastval == 0)
				//if (lastval == 0)
				x = ev[i].value;
				break;
			case 0x35:		//ABS_MT_POSITION_Y
				//if (touch && lastval == 0)
				//if (lastval == 0)
				y = Ysize-ev[i].value;
				break;
			}
			break;
		case EV_KEY:
			if (ev[i].value){
				touch=1;
				b = 1;
				if(b==lastb && ev[i].time.tv_sec == lastt.tv_sec &&
				    (ev[i].time.tv_usec-lastt.tv_usec) < DblTime)
					dbl = 1;
				lastb = b;
				lastt = ev[i].time;
				if(dbl)
					b = b | 1<<8;
			} else {
				touch = 0;
				lastb = b;
				lastt = ev[i].time;
				b = 0;
			}
			break;
		case EV_SYN:
			if (ev[i].code == 0) {
				lastval = 0;
			} else {
				lastval = 1;
			}
			mousetrack(b, x, y, 0);
			return;
		}
	}
}

static void fbreadmouse(void* v)
{
	int rd, value, size = sizeof(struct input_event);
	struct input_event ev[64];
	int i;
	while (1){
		if ((rd = read (eventfd, ev, sizeof(ev))) < size) {
			print("read %d instead of %d\n", rd, size);
			sleep(1);
		}

		for (i = 0; i < rd / size; i++) {
			//print("ev[%d]: type = 0x%x, code = 0x%x, value = 0x%x\n", i, ev[i].type, ev[i].code, ev[i].value);
		}
		touchscreen(ev, (rd / size));
	}
}

uchar* attachscreen ( Rectangle *rect, ulong *chan, int *depth, int *width, int *softscreen )
{
	Xsize &= ~0x3;	/* ensure multiple of 4 */
	rect->min.x = 0;
	rect->min.y = 0;
	rect->max.x = Xsize;
	rect->max.y = Ysize;

	if ( !screeninited ) {
		initscreen ( Xsize, Ysize, &displaychannel, &displaydepth );
		screendata = malloc ( Xsize * Ysize * ( displaydepth / 8  ) );
		if ( !screendata )
			fprint ( 2, "cannot allocate screen buffer" );
	}

	*chan = displaychannel;
	*depth = displaydepth;

	*width = ( Xsize / 4 ) * ( *depth / 8 );
	*softscreen = 1;
	if ( !screeninited ){
		screeninited = 1;
	}

	eventfd = open(mousefile, O_RDONLY);
	kproc("readmouse", fbreadmouse, nil, 0);

	return screendata;
}

void detachscreen ()
{
	free ( screendata );
	screendata = 0;
	framebuffer_release_buffer ( framebuffer );
	framebuffer = 0;
	framebuffer_deinit ();
}

#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))

void flushmemscreen ( Rectangle rect )
{
	if ( !framebuffer || !screendata )
		return;
	int depth = displaydepth / 8;
	int bpl = Xsize * depth;
	int i;
	uchar* fb = framebuffer;
	uchar* screen = screendata;
	int width;
	double angle = 3.14159265 / 2;
	int x,y,u,v;

	if ( rect.min.x < 0 )
		rect.min.x = 0;
	if ( rect.min.y < 0 )
		rect.min.y = 0;
	if ( rect.max.x > Xsize )
		rect.max.x = Xsize;
	if ( rect.max.y > Ysize )
		rect.max.y = Ysize;

	if ( ( rect.max.x < rect.min.x ) || ( rect.max.y < rect.min.y ) )
		return;

	fb += rect.min.y * bpl + rect.min.x * depth;
	screen += rect.min.y * bpl + rect.min.x * depth;
	width = ( rect.max.x - rect.min.x ) * depth;
	for ( i = rect.min.y; i < rect.max.y; i++ ) {
		memcpy ( fb, screen, width );
		fb += bpl;
		screen += bpl;
	}

	if ( ( max ( rect.min.x, pointerposition.x ) > min (rect.max.x, pointerposition.x + pointerwidth ) ) ||
	    ( max ( rect.min.y, pointerposition.y ) > min (rect.max.y, pointerposition.y + pointerheight ) ) )
		return;

	if ( canlock ( &pointerlock ) ) {
		drawpointer ( pointerposition.x , pointerposition.y );
		unlock ( &pointerlock );
	}
}

#include "arrows.h"

static void initscreen ( int aXSize, int aYSize, ulong *chan, int *depth )
{
	if ( framebuffer_init () )
		return;

	framebuffer = framebuffer_get_buffer ();
	switch ( *depth ) {
	case 16: //16bit RGB (2 bytes, red 5@11, green 6@5, blue 5@0) 
		*chan = RGB16;
		pointer = PointerRGB16;
		pointerwidth = PointerRGB16Width;
		pointerheight = PointerRGB16Height;
		break;
	case 24: //24bit RGB (3 bytes, red 8@16, green 8@8, blue 8@0) 
		*chan = RGB24;
		pointer = PointerRGB24;
		pointerwidth = PointerRGB24Width;
		pointerheight = PointerRGB24Height;
		break;
	case 32: //24bit RGB (4 bytes, nothing@24, red 8@16, green 8@8, blue 8@0)
		*chan = XRGB32;
		pointer = PointerRGB32;
		pointerwidth = PointerRGB32Width;
		pointerheight = PointerRGB32Height;
		break;
	}
}

void setpointer ( int x, int y )
{
	if ( !framebuffer || !screendata )
		return;
	int depth = displaydepth / 8;
	int bpl = Xsize * depth;
	int i;
	uchar* fb = framebuffer + pointerposition.y * bpl + pointerposition.x * depth;
	uchar* screen = screendata + pointerposition.y * bpl + pointerposition.x * depth;
	int width = pointerwidth * depth;
	lock ( &pointerlock );
	pointerposition.x = x;
	pointerposition.y = y;
	unlock ( &pointerlock );
	for ( i = 0; i < pointerheight; i++ ) {
		memcpy ( fb, screen, width );
		fb += bpl;
		screen += bpl;
	}
	drawpointer ( pointerposition.x , pointerposition.y );
}

void drawpointer ( int x, int y )
{
	uchar i,j;
	uchar depth = displaydepth / 8;
	uchar* aStart = framebuffer + y * Xsize * depth + x * depth;
	int width = ( ( x + pointerwidth ) < Xsize ) ? pointerwidth : Xsize - x;
	int aHeight = ( ( y + pointerheight ) < Ysize ) ? pointerheight : Ysize - y;
	for ( i = 0; i < aHeight; i++, aStart += Xsize * depth )
		for ( j = 0; j < width * depth; j++ )
			aStart [ j ] &= pointer [ i * depth * pointerwidth + j ];
}
