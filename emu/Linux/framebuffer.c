#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vt.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/vt.h>
#include <linux/kd.h>
#include "framebuffer.h"

static int framebuffer = -1;
struct fb_var_screeninfo screeninfo_orig;
struct fb_var_screeninfo screeninfo_cur;
static int TTY0 = -1;
int TTY = -1;
static unsigned short prevTTY = -1;
static unsigned short activeTTY = -1;
static unsigned short oldframebuffer = -1;
static int oldtermmode = KD_TEXT;
static struct vt_mode oldVTmode;
static struct fb_cursor fbcursor;

static int virtualterminal_init ( );
static void virtualterminal_deinit ( );

int framebuffer_init ( )
{
	int fd = -1;

	if (!access("/dev/tty0", F_OK)) {
		fd = open("/dev/tty0", O_RDWR | O_SYNC);
		if(fd < 0)
			return -1;

		if(ioctl(fd, KDSETMODE, (void*) KD_GRAPHICS)) {
			close(fd);
			return -1;
		}
	}

	framebuffer = open ( "/dev/graphics/fb0", O_RDWR );

	if ( framebuffer < 0 ) {
		fprint ( 2, "can't open a file /dev/fb0: %s\n", strerror ( errno ) );
		return -1;
	}
	/*
  if ( ioctl ( framebuffer, FBIOBLANK, 0 ) ) {
    close ( framebuffer );
    fprint ( 2, "can't blank the framebuffer: %s\n", strerror ( errno ) );
    return -1;
  }
*/

	//  ioctl(framebuffer, FB_ROTATE_UR, 0);

	if ( ioctl ( framebuffer, FBIOGET_VSCREENINFO, &screeninfo_orig ) ) {
		close ( framebuffer );
		fprint ( 2, "can't get the variable screen info: %s\n", strerror ( errno ) );
		return -1;
	}

	screeninfo_cur = screeninfo_orig;
	screeninfo_cur.activate = FB_ACTIVATE_NOW;
	screeninfo_cur.accel_flags = 0;
	if ( ioctl ( framebuffer, FBIOPUT_VSCREENINFO, &screeninfo_cur ) ) {
		close ( framebuffer );
		printf("yuck2\n");
		return -1;
	}

	//  if ( virtualterminal_init ( ) ){
	//    close ( framebuffer );
	//    return -1;
	//  }


	//close ( 0 );
	//dup2 ( TTY, 0 );
	//kbdinit ();
	return 0;
}

void framebuffer_deinit ( )
{
	if ( framebuffer < 0 )
		return;
	virtualterminal_deinit ( );
	if ( ioctl ( framebuffer, FBIOPUT_VSCREENINFO, &screeninfo_orig ) )
		fprint ( 2, "can't put the new variable screen info: %s\n", strerror ( errno ) );
	close ( framebuffer );
}

static char* get_line ( char** currentpos, char* buffend )
{
	char* result = *currentpos;
	while ( *currentpos < buffend ) {
		if  ( **currentpos == '\n' ) {
			**currentpos = 0;
			*currentpos += 1;
			return result;
		}
		if  ( **currentpos == 0 ) {
			*currentpos += 1;
			return result;
		}
		*currentpos += 1;
	}
	return 0;
}

static char* parse_mode ( char* beginmode, char* buffend )
{
	char* linestart = 0;
	int tmp = 0;
	char val [ 10 ] = {
		0	};
	int geomfound = 0;
	int timingsfound = 0;
	do {
		linestart = get_line ( &beginmode, buffend );
		if ( !linestart )
			return beginmode;

		if ( sscanf ( linestart, 
		    " geometry %d %d %d %d %d", 
		    &screeninfo_cur.xres, 
		    &screeninfo_cur.yres, 
		    &tmp, 
		    &tmp, 
		    &screeninfo_cur.bits_per_pixel ) == 5 ) {
			geomfound = 1;
			screeninfo_cur.transp.length = screeninfo_cur.transp.offset = 0;
			screeninfo_cur.sync = 0;
			screeninfo_cur.xoffset = 0;
			screeninfo_cur.yoffset = 0;
			screeninfo_cur.xres_virtual = screeninfo_cur.xres;
			screeninfo_cur.yres_virtual = screeninfo_cur.yres;
			switch ( screeninfo_cur.bits_per_pixel ) {
			case 16:
				screeninfo_cur.red.offset = 11;
				screeninfo_cur.red.length = 5;
				screeninfo_cur.green.offset = 5;
				screeninfo_cur.green.length = 6;
				screeninfo_cur.blue.offset = 0;
				screeninfo_cur.blue.length = 5;
				break;

			case 24:
				screeninfo_cur.red.offset = 16;
				screeninfo_cur.red.length = 8;
				screeninfo_cur.green.offset = 8;
				screeninfo_cur.green.length = 8;
				screeninfo_cur.blue.offset = 0;
				screeninfo_cur.blue.length = 8;
				break;

			case 32:
				screeninfo_cur.red.offset = 16;
				screeninfo_cur.red.length = 8;
				screeninfo_cur.green.offset = 8;
				screeninfo_cur.green.length = 8;
				screeninfo_cur.blue.offset = 0;
				screeninfo_cur.blue.length = 8;
				break;
			}
			continue;
		}
		if ( sscanf ( linestart, 
		    " timings %d %d %d %d %d %d %d", 
		    &screeninfo_cur.pixclock,
		    &screeninfo_cur.left_margin,  
		    &screeninfo_cur.right_margin,
		    &screeninfo_cur.upper_margin, 
		    &screeninfo_cur.lower_margin, 
		    &screeninfo_cur.hsync_len, 
		    &screeninfo_cur.vsync_len ) == 7 ) {
			timingsfound = 1;
			continue;
		}
		if ( ( sscanf ( linestart, 
		    " hsync %6s", 
		    val ) == 1 ) && 
		    !strcmp ( val, "high" ) ) {
			screeninfo_cur.sync |= FB_SYNC_HOR_HIGH_ACT;
			continue;
		}
		if ( ( sscanf ( linestart, 
		    " vsync %6s", 
		    val ) == 1 ) && 
		    !strcmp ( val, "high" ) ) {
			screeninfo_cur.sync |= FB_SYNC_VERT_HIGH_ACT;
			continue;
		}
		if ( ( sscanf ( linestart, 
		    " csync %6s", 
		    val ) == 1 ) && 
		    !strcmp ( val, "high" ) ) {
			screeninfo_cur.sync |= FB_SYNC_COMP_HIGH_ACT ;
			continue;
		}
		if ( ( sscanf ( linestart, 
		    " extsync %6s", 
		    val ) == 1 ) && 
		    !strcmp ( val, "true" ) ) {
			screeninfo_cur.sync |= FB_SYNC_EXT;
			continue;
		}
		if ( ( sscanf ( linestart, 
		    " laced %6s", 
		    val ) == 1 ) && 
		    !strcmp ( val, "true" ) ) {
			screeninfo_cur.sync |= FB_VMODE_INTERLACED;
			continue;
		}
		if ( ( sscanf ( linestart, 
		    " double %6s", 
		    val ) == 1 ) && 
		    !strcmp ( val, "true" ) ) {
			screeninfo_cur.sync |= FB_VMODE_DOUBLE;
			continue;
		}
	}  while ( !strstr ( linestart, "endmode" ) );
	if ( geomfound && timingsfound )
		return 0;
	return beginmode;
}

int framebuffer_enum_modes ( modeCallback callback, 
void* callbackInfo )
{
	int fd = -1;
	struct stat status;
	char* modefile = MAP_FAILED;
	char* linestart = 0;
	char* currentpos = 0;
	char* buffend = 0;
	fd = open ( "/etc/fb.modes", O_RDONLY );
	if ( fd < 0 ) {
		fprint ( 2, "can't open the file /etc/fb.modes: %s\n", strerror ( errno ) );
		return -1;
	}

	if ( fstat ( fd, &status ) < 0 ) {
		fprint ( 2, "can't stat the file /etc/fb.modes: %s\n", strerror ( errno ) );
		close ( fd );
		return -1;
	}

	modefile = mmap ( 0, status.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0 );
	if ( modefile == MAP_FAILED ) {
		fprint ( 2, "can't map /etc/fb.modes: %s\n", strerror ( errno ) );
		close ( fd );
		return -1;
	}
	modefile [ status.st_size ] = 0;
	currentpos = modefile;
	buffend = modefile + status.st_size;
	for ( ;;) {
		char aMode [ 50 ] = {
			0		};
		linestart = get_line ( &currentpos, buffend );
		if ( !linestart )
			return 0;
		if ( sscanf ( linestart, "mode \"%50[^\"]\"", aMode ) == 1 ) {
			do {
				int result = -1;
				int aX = 0;
				int aY = 0;
				int aBitsPerPixel = 0;
				int tmp = 0;
				linestart = get_line ( &currentpos, buffend );
				if ( !linestart ) {
					munmap ( modefile, status.st_size );
					close ( fd );
					return -1;
				}
				result = sscanf ( linestart, 
				    " geometry %d %d %d %d %d", 
				    &aX, 
				    &aY, 
				    &tmp, 
				    &tmp, 
				    &aBitsPerPixel );
				if ( result == 5 ) {
					result = callback ( aX, aY, aBitsPerPixel, callbackInfo );
					if ( result == -1 ) {
						munmap ( modefile, status.st_size );
						close ( fd );
						return -1;
					}
					if ( result == 0 )
						continue;
					currentpos = parse_mode ( linestart, buffend );
					munmap ( modefile, status.st_size );
					close ( fd );
					if ( currentpos )
						return -1;
					if ( ioctl ( framebuffer, FBIOPUT_VSCREENINFO, &screeninfo_cur ) ) {
						fprint ( 2, "can't put the new variable screen info: %s\n", strerror ( errno ) );
						return -1;
					}
					return 0;
				}
			}      while ( !strstr ( linestart, "endmode" ) );
		}
	}
	return 0;
}


unsigned char* framebuffer_get_buffer ( )
{
	struct fb_fix_screeninfo fixedscreeninfo;
	unsigned char* result = 0;
	if ( framebuffer < 0 ) {
		fprint ( 2, "the framebuffer hasn't been initialized" );
		return 0;
	}

	if ( ioctl ( framebuffer, FBIOGET_FSCREENINFO, &fixedscreeninfo ) ) {
		fprint ( 2, "can't the get fixed screen info: %s\n", strerror ( errno ) );
		return 0;
	}

	result = mmap ( 0, fixedscreeninfo.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, framebuffer, 0);
	if ( result == MAP_FAILED ) {
		fprint ( 2, "can't map the framebuffer: %s\n", strerror ( errno ) );
		return 0;
	}
	return result;
}

int framebuffer_get_buffer_size ( )
{
	struct fb_fix_screeninfo fixedscreeninfo;
	unsigned char* result = 0;
	if ( framebuffer < 0 ) {
		fprint ( 2, "the framebuffer hasn't been initialized" );
		return -1;
	}

	if ( ioctl ( framebuffer, FBIOGET_FSCREENINFO, &fixedscreeninfo ) ) {
		fprint ( 2, "can't the get fixed screen info: %s\n", strerror ( errno ) );
		return -1;
	}
	return fixedscreeninfo.smem_len;
}

void framebuffer_release_buffer ( unsigned char* buf )
{
	int length = framebuffer_get_buffer_size ( );
	if ( length == -1 )
		return;
	if ( munmap ( buf, length ) )
		fprint ( 2, "can't release the framebuffer: %s\n", strerror ( errno ) );
	return;
}

void framebuffer_flip_buffer ()
{
	screeninfo_cur.xoffset = 0;
	screeninfo_cur.yoffset = 0;
	screeninfo_cur.activate = FB_ACTIVATE_NOW;
	if ( ioctl ( framebuffer, FBIOPAN_DISPLAY, &screeninfo_cur ) )
		fprint ( 2, "can't flip the framebuffer: %s\n", strerror ( errno ) );
}


static int get_framebuffer4virtualterminal ( int virtualterminal )
{
	struct fb_con2fbmap m;

	m.console = virtualterminal;

	if ( ioctl ( framebuffer, FBIOGET_CON2FBMAP, &m  ) ) {
		fprint ( 2, "can't get framebuffer console: %s\n", strerror ( errno ) );
		return -1;
	}

	return m.framebuffer;
}

static int set_framebuffer4virtualterminal ( int virtualterminal, int fb )
{
	struct fb_con2fbmap m;

	if ( fb >= 0)
		m.framebuffer = fb;
	else {
		struct stat         aStat;
		if ( fstat ( framebuffer, &aStat ) ) {
			fprint ( 2, "can't get status of the framebuffer: %s\n", strerror ( errno ) );
			return -1;
		}
		m.framebuffer = ( aStat.st_rdev & 0xFF ) >> 5;
	}

	m.console = virtualterminal;

	if ( ioctl ( framebuffer, FBIOPUT_CON2FBMAP, &m ) ) {
		fprint ( 2, "can't set a virtual terminal for the framebufer: %s\n", strerror ( errno ) );
		return -1;
	}
	return 0;
}

static int virtualterminal_init ( )
{
	struct vt_stat VTstat;
	char TTYname [ 20 ] = {
		0	};
	if ( ( ( ( TTY0 = open ( "/dev/tty0", O_WRONLY ) ) < 0 ) ) ||
	    ( ( ( TTY0 = open ( "/dev/vc/0", O_RDWR ) ) < 0 ) ) ) {
		fprint ( 2, "can't open terminal: %s\n", strerror ( errno ) );
		return -1;
	}
	if ( ioctl ( TTY0, VT_GETSTATE, &VTstat ) < 0 ) {
		fprint ( 2, "can't open terminal: %s\n", strerror ( errno ) );
		close ( TTY0 );
		return -1;
	}

	prevTTY = VTstat.v_active;

	if ( ( ioctl ( TTY0, VT_OPENQRY, &activeTTY ) == -1 ) || 
	    ( activeTTY == ( unsigned short)-1 ) ) {
		fprint ( 2, "can't open new terminal: %s\n", strerror ( errno ) );
		close ( TTY0 );
		return -1;
	}

	oldframebuffer = get_framebuffer4virtualterminal ( activeTTY );
	set_framebuffer4virtualterminal ( activeTTY, -1 );

	if  ( ioctl ( TTY0, VT_ACTIVATE, activeTTY ) ) {
		fprint ( 2, "can't activate terminal: %s\n", strerror ( errno ) );
		ioctl ( TTY0, VT_DISALLOCATE, activeTTY );
		close ( TTY0 );
		return -1;
	}

	if ( ioctl ( TTY0, VT_WAITACTIVE, activeTTY ) ) {
		fprint ( 2, "can't activate terminal: %s\n", strerror ( errno ) );
		ioctl ( TTY0, VT_DISALLOCATE, activeTTY );
		close ( TTY0 );
		return -1;
	}

	if ( ( snprintf ( TTYname, sizeof ( TTYname ), "/dev/tty%d", activeTTY ) < 0 ) ||
	    ( ( ( TTY = open ( TTYname, O_RDWR ) ) < 0 ) && 
	    ( errno != ENOENT ) ) ||
	    ( snprintf ( TTYname, sizeof ( TTYname ),  "/dev/vc/%d", activeTTY ) < 0 ) ||
	    ( ( TTY = open ( TTYname, O_RDWR ) ) < 0 ) ) {
		fprint ( 2, "can't activate terminal: %s\n", strerror ( errno ) );
		ioctl ( TTY0, VT_ACTIVATE, prevTTY );
		ioctl ( TTY0, VT_WAITACTIVE, prevTTY );
		ioctl ( TTY0, VT_DISALLOCATE, activeTTY );
		close ( TTY0 );
		return -1;
	}

	if ( ioctl ( TTY, KDGETMODE, &oldtermmode ) ) {
		fprint ( 2, "can't get a terminal mode: %s\n", strerror ( errno ) );
		ioctl ( TTY0, VT_ACTIVATE, prevTTY );
		ioctl ( TTY0, VT_WAITACTIVE, prevTTY );
		close ( TTY );
		ioctl ( TTY0, VT_DISALLOCATE, activeTTY );
		close ( TTY0 );
		return -1;
	}

	if ( ioctl ( TTY, KDSETMODE, KD_GRAPHICS ) ) {
		fprint ( 2, "can't set the terminal to the graphics mode: %s\n", strerror ( errno ) );
		ioctl ( TTY0, VT_ACTIVATE, prevTTY );
		ioctl ( TTY0, VT_WAITACTIVE, prevTTY );
		close ( TTY );
		ioctl ( TTY0, VT_DISALLOCATE, activeTTY );
		close ( TTY0 );
		return -1;
	}

	ioctl ( TTY0, TIOCNOTTY, 0 );
	ioctl ( TTY, TIOCSCTTY, 0 );

	const char setCursorOff [] = "\033[?1;0;0c";
	const char setBlankOff [] = "\033[9;0]";

	write ( TTY, setCursorOff, sizeof ( setCursorOff ) );
	write ( TTY, setBlankOff, sizeof ( setBlankOff ) );


	if ( ioctl ( TTY, VT_GETMODE, &oldVTmode ) ) {
		fprint ( 2, "can't get mode for the terminal: %s\n", strerror ( errno ) );
		ioctl ( TTY0, VT_ACTIVATE, prevTTY );
		ioctl ( TTY0, VT_WAITACTIVE, prevTTY );
		close ( TTY );
		ioctl ( TTY0, VT_DISALLOCATE, activeTTY );
		close ( TTY0 );
		return -1;
	}

	return 0;
}

static void virtualterminal_deinit ( )
{
	const char setCursorOn [] = "\033[?0;0;0c";
	const char setBlankOn [] = "\033[9;10]";

	write ( TTY, setCursorOn, sizeof ( setCursorOn ) );
	write ( TTY, setBlankOn, sizeof ( setBlankOn ) );

	ioctl( TTY, VT_SETMODE, &oldVTmode );

	ioctl ( TTY, KDSETMODE, oldtermmode );
	ioctl ( TTY0, VT_ACTIVATE, prevTTY );
	ioctl ( TTY0, VT_WAITACTIVE, prevTTY );
	set_framebuffer4virtualterminal ( activeTTY, oldframebuffer );
	close ( TTY );
	ioctl ( TTY0, VT_ACTIVATE, prevTTY );
	ioctl ( TTY0, VT_WAITACTIVE, prevTTY );
	ioctl ( TTY0, VT_DISALLOCATE, activeTTY );
	close ( TTY0 );
}

