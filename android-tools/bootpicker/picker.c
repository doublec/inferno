#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/kd.h>
/* Android thing */
#include <cutils/properties.h>

void
main(int argc, char *argv[])
{
	char device[128]; // this should be bigger than PROP_NAME_MAX
	char *mousefile;
	int eventfd, fd;
	struct input_event ev[64];
	fd_set set;
	struct timeval timeout;
	int fb = 0;
	int xres, yres, depth;
	unsigned char* screen;
	struct fb_fix_screeninfo fixedscreeninfo;
	struct fb_var_screeninfo screeninfo_orig;
	struct fb_var_screeninfo screeninfo_cur;

	if (!access("/dev/tty0", F_OK)) {
		fd = open("/dev/tty0", O_RDWR | O_SYNC);
		if(fd < 0)
			return -1;

		if(ioctl(fd, KDSETMODE, (void*) KD_GRAPHICS)) {
			close(fd);
			return -1;
		}
	}

	fb = open("/dev/graphics/fb0", O_RDWR);
	if ( fb < 0 ) {
		fprintf( 2, "can't open a file /dev/graphics/fb0" );
		return -1;
	}

	if ( ioctl ( fb, FBIOGET_VSCREENINFO, &screeninfo_orig ) ) {
		close ( fb );
		fprintf ( 2, "can't get the variable screen info" );
		return;
	}

	screeninfo_cur = screeninfo_orig;
	screeninfo_cur.activate = FB_ACTIVATE_NOW;
	screeninfo_cur.accel_flags = 0;
	if ( ioctl ( fb, FBIOPUT_VSCREENINFO, &screeninfo_cur ) ) {
		close ( fb );
		return;
	}

	property_get("ro.product.device", device, "");
	printf("read ro.product.device = %s\n", device);

	if (!strncmp(device, "crespo", 6)) {
		mousefile = "/dev/input/event0";
		xres = 480;
		yres = 800;
		depth = 4;
	} else if (!strncmp(device, "encore", 6)) {
		mousefile = "/dev/input/event2";
		xres = 1024;
		yres = 600;
		depth = 4;
	} else {
		printf("what device are you using? I'm outta here\n");
		exit(1);
	}

	if ( ioctl ( fb, FBIOGET_FSCREENINFO, &fixedscreeninfo ) ) {
		fprintf( 2, "can't the get fixed screen info\n");
		exit(1);
	}

	screen = mmap ( 0, fixedscreeninfo.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fb, 0);
	memset(screen, 0xff, xres*yres*4);
	

	eventfd = open(mousefile, O_RDONLY);

	FD_ZERO(&set);
	FD_SET(eventfd, &set);

	timeout.tv_sec = 3;
	timeout.tv_usec = 0;

	if (select(FD_SETSIZE, &set, NULL, NULL, &timeout)) {
		printf("looks like the screen was touched, boot to Java environment instead\n");
		property_set("inferno.enabled", "0");
	} else {
		property_set("inferno.enabled", "1");
		printf("no input, enable inferno\n");
	}

	memset(screen, 0x00, xres*yres*4);
	close(eventfd);
	close(fb);
}
