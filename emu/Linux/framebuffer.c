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

static int theFrameBuffer = -1;
struct fb_var_screeninfo theOriginVariableScreenInfo;
struct fb_var_screeninfo theCurrentVariableScreenInfo;
static int theTTY0 = -1;
int theTTY = -1;
static unsigned short thePreviousTTY = -1;
static unsigned short theActiveTTY = -1;
static unsigned short theOldFrameBuffer = -1;
static int theOldTerminalMode = KD_TEXT;
static struct vt_mode theOldVTMode;
static struct fb_cursor theCursor;

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

  theFrameBuffer = open ( "/dev/graphics/fb0", O_RDWR );

  if ( theFrameBuffer < 0 ) {
    fprint ( 2, "can't open a file /dev/fb0: %s\n", strerror ( errno ) );
    return -1;
  }
/*
  if ( ioctl ( theFrameBuffer, FBIOBLANK, 0 ) ) {
    close ( theFrameBuffer );
    fprint ( 2, "can't blank the framebuffer: %s\n", strerror ( errno ) );
    return -1;
  }
*/
  if ( ioctl ( theFrameBuffer, FBIOGET_VSCREENINFO, &theOriginVariableScreenInfo ) ) {
    close ( theFrameBuffer );
    fprint ( 2, "can't get the variable screen info: %s\n", strerror ( errno ) );
    return -1;
  }

  theCurrentVariableScreenInfo = theOriginVariableScreenInfo;
  theCurrentVariableScreenInfo.activate = FB_ACTIVATE_NOW;
  theCurrentVariableScreenInfo.accel_flags = 0;
  if ( ioctl ( theFrameBuffer, FBIOPUT_VSCREENINFO, &theCurrentVariableScreenInfo ) ) {
    close ( theFrameBuffer );
    return -1;
  }
  

//  if ( virtualterminal_init ( ) ){
//    close ( theFrameBuffer );
//    return -1;
//  }


  //close ( 0 );
  //dup2 ( theTTY, 0 );
  //kbdinit ();
  return 0;
}

void framebuffer_deinit ( )
{
  if ( theFrameBuffer < 0 )
    return;
  virtualterminal_deinit ( );
  if ( ioctl ( theFrameBuffer, FBIOPUT_VSCREENINFO, &theOriginVariableScreenInfo ) )
    fprint ( 2, "can't put the new variable screen info: %s\n", strerror ( errno ) );
  close ( theFrameBuffer );
}

static char* get_line ( char** aCurrentPosition, char* aBufferEnd )
{
  char* aResult = *aCurrentPosition;
  while ( *aCurrentPosition < aBufferEnd ) {
    if  ( **aCurrentPosition == '\n' ) {
      **aCurrentPosition = 0;
      *aCurrentPosition += 1;
      return aResult;
    }
    if  ( **aCurrentPosition == 0 ) {
      *aCurrentPosition += 1;
      return aResult;
    }
    *aCurrentPosition += 1;
  }
  return 0;
}

static char* parse_mode ( char* aBeginOfMode, char* anEndOfBuffer )
{
  char* aStartOfLine = 0;
  int aTmp = 0;
  char aValue [ 10 ] = {0};
  int aGeometryHasBeenFound = 0;
  int aTimingsHaveBeenFound = 0;
  do {
    aStartOfLine = get_line ( &aBeginOfMode, anEndOfBuffer );
    if ( !aStartOfLine )
      return aBeginOfMode;

    if ( sscanf ( aStartOfLine, 
                  " geometry %d %d %d %d %d", 
                  &theCurrentVariableScreenInfo.xres, 
                  &theCurrentVariableScreenInfo.yres, 
                  &aTmp, 
                  &aTmp, 
                  &theCurrentVariableScreenInfo.bits_per_pixel ) == 5 ) {
      aGeometryHasBeenFound = 1;
      theCurrentVariableScreenInfo.transp.length = theCurrentVariableScreenInfo.transp.offset = 0;
      theCurrentVariableScreenInfo.sync = 0;
      theCurrentVariableScreenInfo.xoffset = 0;
      theCurrentVariableScreenInfo.yoffset = 0;
      theCurrentVariableScreenInfo.xres_virtual = theCurrentVariableScreenInfo.xres;
      theCurrentVariableScreenInfo.yres_virtual = theCurrentVariableScreenInfo.yres;
      switch ( theCurrentVariableScreenInfo.bits_per_pixel ) {
      case 16:
        theCurrentVariableScreenInfo.red.offset = 11;
        theCurrentVariableScreenInfo.red.length = 5;
        theCurrentVariableScreenInfo.green.offset = 5;
        theCurrentVariableScreenInfo.green.length = 6;
        theCurrentVariableScreenInfo.blue.offset = 0;
        theCurrentVariableScreenInfo.blue.length = 5;
        break;

      case 24:
        theCurrentVariableScreenInfo.red.offset = 16;
        theCurrentVariableScreenInfo.red.length = 8;
        theCurrentVariableScreenInfo.green.offset = 8;
        theCurrentVariableScreenInfo.green.length = 8;
        theCurrentVariableScreenInfo.blue.offset = 0;
        theCurrentVariableScreenInfo.blue.length = 8;
        break;

      case 32:
        theCurrentVariableScreenInfo.red.offset = 16;
        theCurrentVariableScreenInfo.red.length = 8;
        theCurrentVariableScreenInfo.green.offset = 8;
        theCurrentVariableScreenInfo.green.length = 8;
        theCurrentVariableScreenInfo.blue.offset = 0;
        theCurrentVariableScreenInfo.blue.length = 8;
        break;
      }
      continue;
    }
    if ( sscanf ( aStartOfLine, 
                  " timings %d %d %d %d %d %d %d", 
                  &theCurrentVariableScreenInfo.pixclock,
                  &theCurrentVariableScreenInfo.left_margin,  
                  &theCurrentVariableScreenInfo.right_margin,
                  &theCurrentVariableScreenInfo.upper_margin, 
                  &theCurrentVariableScreenInfo.lower_margin, 
                  &theCurrentVariableScreenInfo.hsync_len, 
                  &theCurrentVariableScreenInfo.vsync_len ) == 7 ) {
      aTimingsHaveBeenFound = 1;
      continue;
    }
    if ( ( sscanf ( aStartOfLine, 
                    " hsync %6s", 
                    aValue ) == 1 ) && 
         !strcmp ( aValue, "high" ) ) {
      theCurrentVariableScreenInfo.sync |= FB_SYNC_HOR_HIGH_ACT;
      continue;
    }
    if ( ( sscanf ( aStartOfLine, 
                  " vsync %6s", 
                  aValue ) == 1 ) && 
         !strcmp ( aValue, "high" ) ) {
      theCurrentVariableScreenInfo.sync |= FB_SYNC_VERT_HIGH_ACT;
      continue;
    }
    if ( ( sscanf ( aStartOfLine, 
                    " csync %6s", 
                    aValue ) == 1 ) && 
         !strcmp ( aValue, "high" ) ) {
      theCurrentVariableScreenInfo.sync |= FB_SYNC_COMP_HIGH_ACT ;
      continue;
    }
    if ( ( sscanf ( aStartOfLine, 
                    " extsync %6s", 
                    aValue ) == 1 ) && 
         !strcmp ( aValue, "true" ) ) {
      theCurrentVariableScreenInfo.sync |= FB_SYNC_EXT;
      continue;
    }
    if ( ( sscanf ( aStartOfLine, 
                    " laced %6s", 
                    aValue ) == 1 ) && 
         !strcmp ( aValue, "true" ) ) {
      theCurrentVariableScreenInfo.sync |= FB_VMODE_INTERLACED;
      continue;
    }
    if ( ( sscanf ( aStartOfLine, 
                    " double %6s", 
                    aValue ) == 1 ) && 
         !strcmp ( aValue, "true" ) ) {
      theCurrentVariableScreenInfo.sync |= FB_VMODE_DOUBLE;
      continue;
    }
  }
  while ( !strstr ( aStartOfLine, "endmode" ) );
if ( aGeometryHasBeenFound && aTimingsHaveBeenFound )
    return 0;
  return aBeginOfMode;
}

int framebuffer_enum_modes ( modeCallback callbackFunction, 
                             void* callbackInfo )
{
  int aFD = -1;
  struct stat aStatus;
  char* aModesFile = MAP_FAILED;
  char* aStartOfLine = 0;
  char* aCurrentPosition = 0;
  char* anEndOfBuffer = 0;
  aFD = open ( "/etc/fb.modes", O_RDONLY );
  if ( aFD < 0 ) {
    fprint ( 2, "can't open the file /etc/fb.modes: %s\n", strerror ( errno ) );
    return -1;
  }

  if ( fstat ( aFD, &aStatus ) < 0 ) {
    fprint ( 2, "can't stat the file /etc/fb.modes: %s\n", strerror ( errno ) );
    close ( aFD );
    return -1;
  }

  aModesFile = mmap ( 0, aStatus.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, aFD, 0 );
  if ( aModesFile == MAP_FAILED ) {
    fprint ( 2, "can't map /etc/fb.modes: %s\n", strerror ( errno ) );
    close ( aFD );
    return -1;
  }
  aModesFile [ aStatus.st_size ] = 0;
  aCurrentPosition = aModesFile;
  anEndOfBuffer = aModesFile + aStatus.st_size;
  for ( ;;) {
    char aMode [ 50 ] = {0};
    aStartOfLine = get_line ( &aCurrentPosition, anEndOfBuffer );
    if ( !aStartOfLine )
      return 0;
    if ( sscanf ( aStartOfLine, "mode \"%50[^\"]\"", aMode ) == 1 ) {
      do {
        int aResult = -1;
        int aX = 0;
        int aY = 0;
        int aBitsPerPixel = 0;
        int aTmp = 0;
        aStartOfLine = get_line ( &aCurrentPosition, anEndOfBuffer );
        if ( !aStartOfLine ) {
          munmap ( aModesFile, aStatus.st_size );
          close ( aFD );
          return -1;
        }
        aResult = sscanf ( aStartOfLine, 
                           " geometry %d %d %d %d %d", 
                           &aX, 
                           &aY, 
                           &aTmp, 
                           &aTmp, 
                           &aBitsPerPixel );
        if ( aResult == 5 ) {
          aResult = callbackFunction ( aX, aY, aBitsPerPixel, callbackInfo );
          if ( aResult == -1 ) {
            munmap ( aModesFile, aStatus.st_size );
            close ( aFD );
            return -1;
          }
          if ( aResult == 0 )
            continue;
          aCurrentPosition = parse_mode ( aStartOfLine, anEndOfBuffer );
          munmap ( aModesFile, aStatus.st_size );
          close ( aFD );
          if ( aCurrentPosition )
            return -1;
          if ( ioctl ( theFrameBuffer, FBIOPUT_VSCREENINFO, &theCurrentVariableScreenInfo ) ) {
            fprint ( 2, "can't put the new variable screen info: %s\n", strerror ( errno ) );
            return -1;
          }
          return 0;
        }
      }
      while ( !strstr ( aStartOfLine, "endmode" ) );
    }
  }
  return 0;
}


unsigned char* framebuffer_get_buffer ( )
{
  struct fb_fix_screeninfo aFixedScreenInfo;
  unsigned char* aResult = 0;
  if ( theFrameBuffer < 0 ) {
    fprint ( 2, "the framebuffer hasn't been initialized" );
    return 0;
  }

  if ( ioctl ( theFrameBuffer, FBIOGET_FSCREENINFO, &aFixedScreenInfo ) ) {
    fprint ( 2, "can't the get fixed screen info: %s\n", strerror ( errno ) );
    return 0;
  }

  aResult = mmap ( 0, aFixedScreenInfo.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, theFrameBuffer, 0);
  if ( aResult == MAP_FAILED ) {
    fprint ( 2, "can't map the framebuffer: %s\n", strerror ( errno ) );
    return 0;
  }
  return aResult;
}

int framebuffer_get_buffer_size ( )
{
  struct fb_fix_screeninfo aFixedScreenInfo;
  unsigned char* aResult = 0;
  if ( theFrameBuffer < 0 ) {
    fprint ( 2, "the framebuffer hasn't been initialized" );
    return -1;
  }

  if ( ioctl ( theFrameBuffer, FBIOGET_FSCREENINFO, &aFixedScreenInfo ) ) {
    fprint ( 2, "can't the get fixed screen info: %s\n", strerror ( errno ) );
    return -1;
  }
  return aFixedScreenInfo.smem_len;
}

void framebuffer_release_buffer ( unsigned char* aBuffer )
{
  int aLength = framebuffer_get_buffer_size ( );
  if ( aLength == -1 )
    return;
  if ( munmap ( aBuffer, aLength ) )
    fprint ( 2, "can't release the framebuffer: %s\n", strerror ( errno ) );
  return;
}

void framebuffer_flip_buffer ()
{
  theCurrentVariableScreenInfo.xoffset = 0;
  theCurrentVariableScreenInfo.yoffset = 0;
  theCurrentVariableScreenInfo.activate = FB_ACTIVATE_NOW;
  if ( ioctl ( theFrameBuffer, FBIOPAN_DISPLAY, &theCurrentVariableScreenInfo ) )
    fprint ( 2, "can't flip the framebuffer: %s\n", strerror ( errno ) );
}


static int get_framebuffer4virtualterminal ( int aVirtualTerminal )
{
  struct fb_con2fbmap aMap;

  aMap.console = aVirtualTerminal;

  if ( ioctl ( theFrameBuffer, FBIOGET_CON2FBMAP, &aMap  ) ) {
    fprint ( 2, "can't get framebuffer console: %s\n", strerror ( errno ) );
    return -1;
  }
  
  return aMap.framebuffer;
}

static int set_framebuffer4virtualterminal ( int aVirtualTerminal, int aFrameBuffer )
{
  struct fb_con2fbmap aMap;

  if ( aFrameBuffer >= 0) 
    aMap.framebuffer = aFrameBuffer;
  else {
    struct stat         aStat;
    if ( fstat ( theFrameBuffer, &aStat ) ) {
      fprint ( 2, "can't get status of the framebuffer: %s\n", strerror ( errno ) );    
      return -1;
    }
    aMap.framebuffer = ( aStat.st_rdev & 0xFF ) >> 5;
  }

  aMap.console = aVirtualTerminal;
  
  if ( ioctl ( theFrameBuffer, FBIOPUT_CON2FBMAP, &aMap ) ) {
    fprint ( 2, "can't set a virtual terminal for the framebufer: %s\n", strerror ( errno ) ); 
    return -1;
  }
  return 0;
}

static int virtualterminal_init ( )
{
  struct vt_stat aVT_Stat;
  char aTTYName [ 20 ] = {0};
  if ( ( ( ( theTTY0 = open ( "/dev/tty0", O_WRONLY ) ) < 0 ) ) ||
       ( ( ( theTTY0 = open ( "/dev/vc/0", O_RDWR ) ) < 0 ) ) ) {
    fprint ( 2, "can't open terminal: %s\n", strerror ( errno ) );    
    return -1;
  }
  if ( ioctl ( theTTY0, VT_GETSTATE, &aVT_Stat ) < 0 ) {
    fprint ( 2, "can't open terminal: %s\n", strerror ( errno ) );    
    close ( theTTY0 );
    return -1;
  }
  
  thePreviousTTY = aVT_Stat.v_active;

  if ( ( ioctl ( theTTY0, VT_OPENQRY, &theActiveTTY ) == -1 ) || 
       ( theActiveTTY == ( unsigned short)-1 ) ) {
    fprint ( 2, "can't open new terminal: %s\n", strerror ( errno ) );    
    close ( theTTY0 );
    return -1;
  }
  
  theOldFrameBuffer = get_framebuffer4virtualterminal ( theActiveTTY );
  set_framebuffer4virtualterminal ( theActiveTTY, -1 );
  
  if  ( ioctl ( theTTY0, VT_ACTIVATE, theActiveTTY ) ) {
    fprint ( 2, "can't activate terminal: %s\n", strerror ( errno ) );    
    ioctl ( theTTY0, VT_DISALLOCATE, theActiveTTY );
    close ( theTTY0 );
    return -1;
  }

  if ( ioctl ( theTTY0, VT_WAITACTIVE, theActiveTTY ) ) {
    fprint ( 2, "can't activate terminal: %s\n", strerror ( errno ) );    
    ioctl ( theTTY0, VT_DISALLOCATE, theActiveTTY );
    close ( theTTY0 );
    return -1;
  }
  
  if ( ( snprintf ( aTTYName, sizeof ( aTTYName ), "/dev/tty%d", theActiveTTY ) < 0 ) ||
       ( ( ( theTTY = open ( aTTYName, O_RDWR ) ) < 0 ) && 
         ( errno != ENOENT ) ) ||
       ( snprintf ( aTTYName, sizeof ( aTTYName ),  "/dev/vc/%d", theActiveTTY ) < 0 ) ||
       ( ( theTTY = open ( aTTYName, O_RDWR ) ) < 0 ) ) {
    fprint ( 2, "can't activate terminal: %s\n", strerror ( errno ) );    
    ioctl ( theTTY0, VT_ACTIVATE, thePreviousTTY );
    ioctl ( theTTY0, VT_WAITACTIVE, thePreviousTTY );
    ioctl ( theTTY0, VT_DISALLOCATE, theActiveTTY );
    close ( theTTY0 );
    return -1;
  }

  if ( ioctl ( theTTY, KDGETMODE, &theOldTerminalMode ) ) {
    fprint ( 2, "can't get a terminal mode: %s\n", strerror ( errno ) );    
    ioctl ( theTTY0, VT_ACTIVATE, thePreviousTTY );
    ioctl ( theTTY0, VT_WAITACTIVE, thePreviousTTY );
    close ( theTTY );
    ioctl ( theTTY0, VT_DISALLOCATE, theActiveTTY );
    close ( theTTY0 );
    return -1;
  }
  
  if ( ioctl ( theTTY, KDSETMODE, KD_GRAPHICS ) ) {
    fprint ( 2, "can't set the terminal to the graphics mode: %s\n", strerror ( errno ) );    
    ioctl ( theTTY0, VT_ACTIVATE, thePreviousTTY );
    ioctl ( theTTY0, VT_WAITACTIVE, thePreviousTTY );
    close ( theTTY );
    ioctl ( theTTY0, VT_DISALLOCATE, theActiveTTY );
    close ( theTTY0 );
    return -1;
  }
  
  ioctl ( theTTY0, TIOCNOTTY, 0 );
  ioctl ( theTTY, TIOCSCTTY, 0 );
  
  const char setCursorOff [] = "\033[?1;0;0c";
  const char setBlankOff [] = "\033[9;0]";

  write ( theTTY, setCursorOff, sizeof ( setCursorOff ) );
  write ( theTTY, setBlankOff, sizeof ( setBlankOff ) );


  if ( ioctl ( theTTY, VT_GETMODE, &theOldVTMode ) ) {
    fprint ( 2, "can't get mode for the terminal: %s\n", strerror ( errno ) );    
    ioctl ( theTTY0, VT_ACTIVATE, thePreviousTTY );
    ioctl ( theTTY0, VT_WAITACTIVE, thePreviousTTY );
    close ( theTTY );
    ioctl ( theTTY0, VT_DISALLOCATE, theActiveTTY );
    close ( theTTY0 );
    return -1;
  }

  return 0;
}  

static void virtualterminal_deinit ( )
{
  const char setCursorOn [] = "\033[?0;0;0c";
  const char setBlankOn [] = "\033[9;10]";
     
  write ( theTTY, setCursorOn, sizeof ( setCursorOn ) );
  write ( theTTY, setBlankOn, sizeof ( setBlankOn ) );

  ioctl( theTTY, VT_SETMODE, &theOldVTMode );
     
  ioctl ( theTTY, KDSETMODE, theOldTerminalMode );
  ioctl ( theTTY0, VT_ACTIVATE, thePreviousTTY );
  ioctl ( theTTY0, VT_WAITACTIVE, thePreviousTTY );
  set_framebuffer4virtualterminal ( theActiveTTY, theOldFrameBuffer );
  close ( theTTY );
  ioctl ( theTTY0, VT_ACTIVATE, thePreviousTTY );
  ioctl ( theTTY0, VT_WAITACTIVE, thePreviousTTY );
  ioctl ( theTTY0, VT_DISALLOCATE, theActiveTTY );
  close ( theTTY0 );
}

