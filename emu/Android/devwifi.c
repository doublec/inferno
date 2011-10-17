/* TODO/BUGS in no order:
   should send acknowledgement when an sms is read from /phone/sms, not just on every read
   consider notifying RIL of screen state using RIL_REQUEST_SCREEN_STATE to conserve power
   too many assumptions made in loop_for_data() about read() giving exactly the right amount of data
   parcel code will go crazy if ints overflow
   need to support DTMF
*/

#include "dat.h"
#include "fns.h"
#include "error.h"
#include "version.h"
#include "mp.h"
#include "libsec.h"
#include "keyboard.h"
#include <stdio.h>
#include <cutils/sockets.h>
#include <stdlib.h>
#include <cutils/jstring.h>
#include <arpa/inet.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>

// Inferno-related globals

enum
{
	Qdir,
	Qctl
};

Dirtab wifitab[] =
{
	".",	  {Qdir, 0, QTDIR}, 0, DMDIR|0555,
	"wifictl",	  {Qctl},	    0, 0666,
};

static struct {
	QLock l;
	Rendez r;
	int ready;
	int used;
	char *msg;
} status_msg;

#define DPRINT if(1) print

void
wifiinit(void)
{
	int pid;

	pid = fork();

	if (pid == 0) {
		chdir("/data/misc/wifi");
		execl("/system/bin/wpa_supplicant", "wpa_supplicant", "-D", "wext", "-i", "eth0", "-c", "/data/misc/wifi/wpa_supplicant.conf", NULL);
	}
//	kproc("phone", loop_for_data, 0, 0);
}

static Chan*
wifiattach(char *spec)
{
	// setup kprocs if necessary
	return devattach('w', spec);
}

static Walkqid*
wifiwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, wifitab, nelem(wifitab), devgen);
}

static int
wifistat(Chan *c, uchar *db, int n)
{
	return devstat(c, db, n, wifitab, nelem(wifitab), devgen);
}

static Chan*
wifiopen(Chan *c, int omode)
{
	c = devopen(c, omode, wifitab, nelem(wifitab), devgen);

	return c;
}

static void
wificlose(Chan *c)
{
	if((c->flag & COPEN) == 0)
		return;
}

static long
wifiread(Chan *c, void *va, long n, vlong offset)
{
	char buf[255];
	int i, str_offset;
	if(c->qid.type & QTDIR) {
		return devdirread(c, va, n, wifitab, nelem(wifitab), devgen);
	}

	switch((ulong) c->qid.path) {
	case Qctl:
//		return readstr(offset, va, n, power_state ? "on\n" : "off\n");
		return 0;
	}
	return 0;
}

static long
wifiwrite(Chan *c, void *va, long n, vlong offset)
{
	char *pdu;
	Rune *runestr;
	int i, j;
	Cmdbuf *cmd;

	if(c->qid.type & QTDIR)
		error(Eperm);

	cmd = parsecmd(va, n);
	if(waserror()) {
		free(cmd);
		nexterror();
		return 0;
	}

	switch((ulong) c->qid.path) {
	case Qctl:
		// Here we just look for which commands were sent. The nesting
		// can get pretty ugly for commands with a lot of parameters
		// (sorry!)
//		if(strcmp(cmd->f[0], "on") == 0) {
//			radio_power(1);
//		} else if(strcmp(cmd->f[0], "off") == 0) {
//			radio_power(0);
//		} else if(strcmp(cmd->f[0], "net") == 0) {
//			if(cmd->nf >= 2) {
//				if(strcmp(cmd->f[1], "on") == 0) {
//					// Ugly to do a different thing for each
//					// # of parameters, but cmd->f is
//					// dynamically allocated so we can't
//					// just set cmd->f[cmd-nf:end] to NULL
//					switch(cmd->nf) {
//					case 2: activate_net(NULL, NULL, NULL, NULL);
//						break;
//					case 3: activate_net(cmd->f[2], NULL, NULL, NULL);
//						break;
//					case 4: activate_net(cmd->f[2], cmd->f[3], NULL, NULL);
//						break;
//					case 5: activate_net(cmd->f[2], cmd->f[3], cmd->f[4], NULL);
//						break;
//					default: activate_net(cmd->f[2], cmd->f[3], cmd->f[4], cmd->f[5]);
//						break;
//					}
//				} else if(strcmp(cmd->f[1], "off") == 0) {
//					deactivate_net();
//				}
//			}
//		} else if(strcmp(cmd->f[0], "mute") == 0) {
//			set_mute(1);
//		} else if(strcmp(cmd->f[0], "unmute") == 0) {
//			set_mute(0);
//		} else if(strcmp(cmd->f[0], "route") == 0) {
//			if(cmd->nf >= 2) {
//				if(strcmp(cmd->f[1], "earpiece") == 0) {
//					af_setParameters("routing=1");
//				} else if(strcmp(cmd->f[1], "speaker") == 0) {
//					af_setParameters("routing=2");
//				} else if(strcmp(cmd->f[1], "headphone") == 0) {
//					af_setParameters("routing=3");
//				}
//			}
//		} else if(strcmp(cmd->f[0], "volume") == 0) {
//			if(cmd->nf >= 2) {
//				switch(strtol(cmd->f[1], NULL, 10)) {
//				case 0: af_setVoiceVolume(0.0); break;
//				case 1: af_setVoiceVolume(0.2); break;
//				case 2: af_setVoiceVolume(0.4); break;
//				case 3: af_setVoiceVolume(0.6); break;
//				case 4: af_setVoiceVolume(0.8); break;
//				case 5: af_setVoiceVolume(1.0); break;
//				default: af_setVoiceVolume(1.0); break;
//				}
//			}
//		}
		break;
	}
	poperror();
	free(cmd);
	return n;
}

Dev wifidevtab = {
	'w',
	"wifi",

	wifiinit,
	wifiattach,
	wifiwalk,
	wifistat,
	wifiopen,
	devcreate,
	wificlose,
	wifiread,
	devbread,
	wifiwrite,
	devbwrite,
	devremove,
	devwstat
};
