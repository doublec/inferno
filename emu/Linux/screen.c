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

static int eventfd, /*mainbuttonfd, */volbuttonfd;
static int mousepid;

static int b = 0;
static int DblTime = 300000;
static int x, y, p, dbl = 0;
static int lastb;
static int touch = 0;

static int xmin = 2;
static int xadjust = 3; //shifts mouse left
static int lowx = 5; //should be xmin + xadjust
static int ymin = 2;
static int yadjust = 11; //shifts mouse up
static int lowy = 13; //should be ymin + yadjust

static void 
touchscreen(struct input_event* ev, int count)
{
    int i;
    static struct timeval lastt;

    for (i=0; i < count; i++){
	//printf("ev[%d]: type = 0x%x, code = 0x%x, value = 0x%x, touch = %d\n", i, ev[i].type, ev[i].code, ev[i].value, touch);
	if(0) {
		fprint(2, "%d/%d [%d] ", i, count, ev[i].code);
	}
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
		if (ev[i].value) {
		    touch = 1;
		    b = 1;
		} else if (ev[i].value == 0) {
		    touch = 0;
		    b = 0;
		}
		break;
	    case 0x36:		// ABS_MT_POSITION_X
		if (type == 'c' | type != 's') {
			if(!rotation_opt) {
				if (ev[i].value > lowx)
					x = ev[i].value - xadjust;
				else
					x = xmin;
			} else {
				if (ev[i].value > lowy)
					y = ev[i].value - yadjust;
				else
					y = ymin;
			}
		}
		if (type == 's') {
			if(!rotation_opt) {
				if ((ev[i].value * 800 / 1024) > lowy)
					y = (ev[i].value * 800 /1024) - yadjust;
				else
					y = ymin;

			} else {
				if ((ev[i].value * 800 / 1024) > lowx)
					x = (ev[i].value * 800 / 1024) - xadjust;
				else
					x = xmin;
			}
		}
		break;
	    case 0x35:		//ABS_MT_POSITION_Y
		if (type == 'n' | type != 's') {
			if(!rotation_opt) {
				if (Ysize-ev[i].value > lowy)
					y = Ysize-ev[i].value - yadjust;
				else
					y = ymin;
				break;
			} else {
				if (ev[i].value > lowx)
					x = ev[i].value - xadjust;
				else
					x = xmin;
			}
		}
		if (type == 's') {
			if(!rotation_opt) {
				if ((ev[i].value * 480 /1024) > lowx)
					x = (ev[i].value * 480 /1024) - xadjust;
				else
					x = xmin;
			} else {
				if ((Ysize-ev[i].value * 480 /1024) > lowy)
					y = (Ysize-ev[i].value * 480 / 1024) - yadjust;
				else
					y = ymin;
			}
		}
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
	    if (ev[i].code == 30) {
		if (ev[i].value == 0) {
			return;
		}
	    }
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
    int i, j;
    uchar* fb = framebuffer;
    uchar* screen = screendata;
    int width;

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
    if(rotation_opt) {
	uchar *cur_col;
	uchar *fb_begin = fb;
	uchar *scr_begin = screen;
	int fb_bpl = Ysize * depth;
	int fb_startx, fb_starty;
	fb_starty = Xsize - rect.min.x;
	fb_startx = rect.min.y;
	fb += fb_starty * fb_bpl + fb_startx * depth;
	screen += rect.min.y * bpl + rect.min.x * depth;
	cur_col = fb;
	for(i = rect.min.y; i < rect.max.y; i++) {
	    fb = cur_col;
	    for(j = rect.min.x; j < rect.max.x; j++) {
		memcpy(fb, screen, depth);
		fb -= fb_bpl;
		screen += depth;
	    }
	    cur_col += depth;
	    screen += depth * ((Xsize - rect.max.x) + rect.min.x);
	}
    } else {
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
    if(rotation_opt) {
	int depth = displaydepth / 8;
	int bpl = Xsize * depth;
	int fb_bpl = Ysize * depth;
	int i, j;
	uchar *cur_col;
	uchar *fb = framebuffer + (Xsize - pointerposition.x) * fb_bpl + pointerposition.y * depth;
	uchar *screen = screendata + pointerposition.y * bpl + (pointerposition.x) * depth;
	int height = (pointerposition.y + pointerheight > Ysize) ? Ysize - pointerposition.y : pointerheight;
	lock(&pointerlock);
	pointerposition.x = x;
	pointerposition.y = y;
	unlock(&pointerlock);
	cur_col = fb;
	for(i = 0; i < height; i++) {
	    fb = cur_col;
	    for(j = 0; j < pointerwidth; j++) {
		memcpy(fb, screen, depth);
		fb -= fb_bpl;
		screen += depth;
	    }
	    cur_col += depth;
	    screen += depth*(Xsize - pointerwidth);
	}
	drawpointer(pointerposition.x, pointerposition.y);
    } else {
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
}

void drawpointer ( int x, int y )
{
    uchar i,j;
    uchar depth = displaydepth / 8;
    uchar* aStart = framebuffer + y * Xsize * depth + x * depth;
    int width = ( ( x + pointerwidth ) < Xsize ) ? pointerwidth : Xsize - x;
    int aHeight = ( ( y + pointerheight ) < Ysize ) ? pointerheight : Ysize - y;
    if(rotation_opt) {
	uchar *cur_col;
	uchar *fb = framebuffer;
	int fb_bpl = Ysize * depth;
	int fb_startx, fb_starty;
	char empty[depth];
	char *pointerptr = pointer;
	for(i = 0; i < depth; i++) {
	    empty[i] = 0xFF; // initialize white space comparison
	}
	fb_starty = Xsize - x;
	fb_startx = y;
	fb += fb_starty * fb_bpl + fb_startx * depth;
	cur_col = fb;
	for(i = 0; i < aHeight; i++) {
	    fb = cur_col;
	    for(j = 0; j < width; j++) {
		if(memcmp(empty, pointerptr, depth)) {
		    memcpy(fb, pointerptr, depth);
		}
		fb -= fb_bpl;
		pointerptr += depth;
	    }
	    cur_col += depth;
	}
    } else {
	for ( i = 0; i < aHeight; i++, aStart += Xsize * depth )
	    for ( j = 0; j < width * depth; j++ )
		aStart [ j ] &= pointer [ i * depth * pointerwidth + j ];
    }
}
