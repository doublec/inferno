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

static ulong theDisplayChannel = 0;
static int theDisplayDepth = 0;
static int theScreenIsInited = 0;
static unsigned char* theScreenData = 0;
static unsigned char* theFrameBuffer = 0;
static Point thePointerPosition = { 0, 0 };
static Lock thePointerLock;
static uchar* thePointer = 0;
static int thePointerHeight = 0;
static int thePointerWidth = 0;

static void screenProc ( void* );
static void initScreen ( int, int, ulong*, int* );
void drawPointer ( int, int );

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
					if (ev[i].value && lastval == 0) {
						touch = 1;
						b = 1;
					} else if (ev[i].value == 0 && lastval == 0) {
						touch = 0;
						b = 0;
					}
					//lastval = ev[i].value;
				case 0x36:		// ABS_MT_POSITION_X
					//if (touch && lastval == 0)
					if (lastval == 0)
						x = ev[i].value;
					break;
				case 0x35:		//ABS_MT_POSITION_Y
					//if (touch && lastval == 0)
					if (lastval == 0)
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
//				print("touch = %d, b = %d\n", touch, b);
/*
				if (i == 2) {
					mousetrack(b, x, y, 0);
				} else if (i > 2 && touch) {
					mousetrack(b, x, y, 0);
				} else if (!touch && lastb) {
					mousetrack(b, x, y, 0);
				}
*/
				mousetrack(b, x, y, 0);
//				if (i==2 && p>0)		// motion
//					mousetrack (b, x, y, 0);
//				else if (i>3 && p>0 && touch)	// press
//					mousetrack(b, x, y, 0);
//				else if (i > 3 && touch) // press
//					mousetrack(b, x, y, 1);
//				else if (i>3 && p==0 && !touch)	// release
//					mousetrack(0, 0, 0, 1);
/*
				if (touch) {
					touch = 0;
					b = 0;
				} else {
					touch = 1;
					b = 1;
				}
*/
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
      if ((rd = read (eventfd, ev, sizeof(ev))) < size)
          //perror_exit ("read()");     
	print("gaaack\n");

	for (i = 0; i < rd / size; i++) {
		print("ev[%d]: type = 0x%x, code = 0x%x, value = 0x%x\n", i, ev[i].type, ev[i].code, ev[i].value);
	}
	touchscreen(ev, (rd / size));
  }
}

uchar* attachscreen ( Rectangle *aRectangle, ulong *aChannel, int *aDepth, int *aWidth, int *aSoftScreen )
{
  Xsize &= ~0x3;	/* ensure multiple of 4 */
  aRectangle->min.x = 0;
  aRectangle->min.y = 0;
  aRectangle->max.x = Xsize;
  aRectangle->max.y = Ysize;

  theDisplayDepth = 32;
 
  if ( !theScreenIsInited ) {
    initScreen ( Xsize, Ysize, &theDisplayChannel, &theDisplayDepth );
    theScreenData = malloc ( Xsize * Ysize * ( theDisplayDepth / 8  ) );
    if ( !theScreenData )
      fprint ( 2, "cannot allocate screen buffer" );
  }

  *aChannel = theDisplayChannel;
  *aDepth = theDisplayDepth;

  *aWidth = ( Xsize / 4 ) * ( *aDepth / 8 );
  *aSoftScreen = 1;
  if ( !theScreenIsInited ){
    theScreenIsInited = 1;
  }

  eventfd = open("/dev/input/event2", O_RDONLY);
  kproc("readmouse", fbreadmouse, nil, 0);

  return theScreenData;
}

void detachscreen ()
{
  free ( theScreenData );
  theScreenData = 0;
  framebuffer_release_buffer ( theFrameBuffer );
  theFrameBuffer = 0;
  framebuffer_deinit ();
}

#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))

void flushmemscreen ( Rectangle aRectangle )
{
  if ( !theFrameBuffer || !theScreenData )
    return;
  int aDepth = theDisplayDepth / 8;
  int aBytesPerLine = Xsize * aDepth;
  int i;
  uchar* aFrameBuffer = theFrameBuffer;
  uchar* aScreenData = theScreenData;
  int aWidth;

  if ( aRectangle.min.x < 0 )
    aRectangle.min.x = 0;
  if ( aRectangle.min.y < 0 )
    aRectangle.min.y = 0;
  if ( aRectangle.max.x > Xsize )
    aRectangle.max.x = Xsize;
  if ( aRectangle.max.y > Ysize )
    aRectangle.max.y = Ysize;
  
  if ( ( aRectangle.max.x < aRectangle.min.x ) || ( aRectangle.max.y < aRectangle.min.y ) )
    return;

  aFrameBuffer += aRectangle.min.y * aBytesPerLine + aRectangle.min.x * aDepth;
  aScreenData += aRectangle.min.y * aBytesPerLine + aRectangle.min.x * aDepth;
  aWidth = ( aRectangle.max.x - aRectangle.min.x ) * aDepth;
  for ( i = aRectangle.min.y; i < aRectangle.max.y; i++ ) {
    memcpy ( aFrameBuffer, aScreenData, aWidth );
    aFrameBuffer += aBytesPerLine;
    aScreenData += aBytesPerLine;
  }

  if ( ( max ( aRectangle.min.x, thePointerPosition.x ) > min (aRectangle.max.x, thePointerPosition.x + thePointerWidth ) ) ||
       ( max ( aRectangle.min.y, thePointerPosition.y ) > min (aRectangle.max.y, thePointerPosition.y + thePointerHeight ) ) ) 
    return;
  
  if ( canlock ( &thePointerLock ) ) {
    drawPointer ( thePointerPosition.x , thePointerPosition.y );
    unlock ( &thePointerLock );
  }
}

int choiseModeCallback ( int x, int y, int bpp, int* callbackInfo )
{
  if ( ( x == Xsize ) && ( y == Ysize ) && ( *callbackInfo < ( bpp ) ) )
    *callbackInfo = bpp;
  return 0;
}

int selectModeCallback ( int x, int y, int bpp, int* callbackInfo )
{
  if ( ( x == Xsize ) && ( y == Ysize ) && ( *callbackInfo == ( bpp ) ) )
    return 1;
  return 0;
}

#include "arrows.h"

static void initScreen ( int aXSize, int aYSize, ulong *aChannel, int *aDepth )
{
  if ( framebuffer_init () )
    return;
/* 
  if ( framebuffer_enum_modes ( (modeCallback)choiseModeCallback, aDepth ) )
    return;

  if ( framebuffer_enum_modes ( (modeCallback)selectModeCallback, aDepth ) )
    return;
*/
  theFrameBuffer = framebuffer_get_buffer ();
  switch ( *aDepth ) {
  case 16: //16bit RGB (2 bytes, red 5@11, green 6@5, blue 5@0) 
    *aChannel = RGB16;
    thePointer = PointerRGB16;
    thePointerWidth = PointerRGB16Width;
    thePointerHeight = PointerRGB16Height;
    break;
  case 24: //24bit RGB (3 bytes, red 8@16, green 8@8, blue 8@0) 
    *aChannel = RGB24;
    thePointer = PointerRGB24;
    thePointerWidth = PointerRGB24Width;
    thePointerHeight = PointerRGB24Height;
    break;
  case 32: //24bit RGB (4 bytes, nothing@24, red 8@16, green 8@8, blue 8@0)
    *aChannel = XRGB32;
    thePointer = PointerRGB32;
    thePointerWidth = PointerRGB32Width;
    thePointerHeight = PointerRGB32Height;
    break;
  }
}

void setpointer ( int x, int y )
{
  if ( !theFrameBuffer || !theScreenData )
    return;
  int aDepth = theDisplayDepth / 8;
  int aBytesPerLine = Xsize * aDepth;
  int i;
  uchar* aFrameBuffer = theFrameBuffer + thePointerPosition.y * aBytesPerLine + thePointerPosition.x * aDepth;
  uchar* aScreenData = theScreenData + thePointerPosition.y * aBytesPerLine + thePointerPosition.x * aDepth;
  int aWidth = thePointerWidth * aDepth;
  lock ( &thePointerLock );
  thePointerPosition.x = x;
  thePointerPosition.y = y;
  unlock ( &thePointerLock );
  for ( i = 0; i < thePointerHeight; i++ ) {
    memcpy ( aFrameBuffer, aScreenData, aWidth );
    aFrameBuffer += aBytesPerLine;
    aScreenData += aBytesPerLine;
  }
  drawPointer ( thePointerPosition.x , thePointerPosition.y );  
}

void drawPointer ( int x, int y )
{
  uchar i,j;
  uchar aDepth = theDisplayDepth / 8;
  uchar* aStart = theFrameBuffer + y * Xsize * aDepth + x * aDepth;
  int aWidth = ( ( x + thePointerWidth ) < Xsize ) ? thePointerWidth : Xsize - x;
  int aHeight = ( ( y + thePointerHeight ) < Ysize ) ? thePointerHeight : Ysize - y;
  for ( i = 0; i < aHeight; i++, aStart += Xsize * aDepth )
    for ( j = 0; j < aWidth * aDepth; j++ ) 
      aStart [ j ] &= thePointer [ i * aDepth * thePointerWidth + j ];
}
