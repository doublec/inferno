implement Dialer;

include "sys.m";
	sys: Sys;

include "draw.m";
	draw: Draw;

include "tk.m";
	tk: Tk;

include "tkclient.m";
	tkclient: Tkclient;

include "dialog.m";
	dialog: Dialog;

include "string.m";
	str: String;

Dialer: module {
	init: fn(ctxt: ref Draw->Context, argv: list of string);
};

IDLE, RINGING, ACTIVE : con iota;
phone_state := IDLE;

EARPIECE, SPEAKER : con iota;
audio_route := EARPIECE;

task_cfg := array[] of {
	"frame .fl -width 40w -height 40w",
	"frame .b",
	"label .l -text {Enter number to dial}",
	"button .b.back -text {<-} -command {send cmd back}",
	"frame .row1 -height 100",
	"button .b.1 -height 50 -text 1 -command {send cmd 1}",
	"button .b.2 -height 50 -text 2 -command {send cmd 2}",
	"button .b.3 -height 50 -text 3 -command {send cmd 3}",
	"pack .b.1 .b.2 .b.3 -in .row1 -side left -fill both -expand true",
	"frame .row2 -height 100",
	"button .b.4 -height 50 -text 4 -command {send cmd 4}",
	"button .b.5 -height 50 -text 5 -command {send cmd 5}",
	"button .b.6 -height 50 -text 6 -command {send cmd 6}",
	"pack .b.4 .b.5 .b.6 -in .row2 -side left -fill both -expand true",
	"frame .row3 -height 100",
	"button .b.7 -height 50 -text 7 -command {send cmd 7}",
	"button .b.8 -height 50 -text 8 -command {send cmd 8}",
	"button .b.9 -height 50 -text 9 -command {send cmd 9}",
	"pack .b.7 .b.8 .b.9 -in .row3 -side left -fill both -expand true",
	"frame .row4 -height 100",
	"button .b.* -height 50 -text * -command {send cmd *}",
	"button .b.0 -height 50 -text 0 -command {send cmd 0}",
	"button .b.# -height 50 -text # -command {send cmd #}",
	"pack .b.* .b.0 .b.# -in .row4 -side left -fill both -expand true",
	"button .b.dial -height 50 -text Dial -command {send cmd dial}",
	"button .b.hangup -height 50 -text {Hang up} -command {send cmd hangup}",
	"button .b.switchroute -height 50 -text {Speaker} -command {send cmd switch_route}",
	"pack .l .b.back",
	"pack .row1 .row2 .row3 .row4 -fill both -in .b",
	"pack .b.dial .b.hangup .b.switchroute -fill both -in .b",
	"pack .b -fill both",
	"pack .fl -fill both -expand 1",
	"pack propagate . 0",
	"update",
};

init(ctxt: ref Draw->Context, argv: list of string)
{
	sys = load Sys Sys->PATH;
	draw = load Draw Draw->PATH;
	tk = load Tk Tk->PATH;
	tkclient = load Tkclient Tkclient->PATH;
	dialog = load Dialog Dialog->PATH;
	str = load String String->PATH;

	labeltext := "";

	tkclient->init();
	dialog->init();

	(t, wmctl) := tkclient->toplevel(ctxt, "", "Dialer", Tkclient->Appl);
	if(t == nil)
		return;

	cmd := chan of string;
	tk->namechan(t, cmd, "cmd");

	for (c:=0; c<len task_cfg; c++)
		tk->cmd(t, task_cfg[c]);

	tkclient->onscreen(t, nil);
	tkclient->startinput(t, "kbd"::"ptr"::nil);

	phonech := chan of string;
	spawn monitor(phonech);
	spawn monitor_buttons();

	if(len argv > 1) {
		# we were given a number as an argument, start dialing
		number := hd tl argv;
		labeltext = number;
		dial(number);
	}

	for(;;)	alt {
	s := <-t.ctxt.kbd =>
		tk->keyboard(t, s);
	s := <-t.ctxt.ptr =>
		tk->pointer(t, *s);
	s := <-t.ctxt.ctl or
	s = <-t.wreq =>
		tkclient->wmctl(t, s);
	menu := <-wmctl =>
		case menu {
		"exit" =>
			return;
		* =>
			tkclient->wmctl(t, menu);
		}
	bcmd := <-cmd =>
		case bcmd {
		"1" =>
		sys->print("1");
		labeltext = labeltext + "1";
		"2" =>
		sys->print("2");
		labeltext = labeltext + "2";
		"3" =>
		sys->print("3");
		labeltext = labeltext + "3";
		"4" =>
		sys->print("4");
		labeltext = labeltext + "4";
		"5" =>
		sys->print("5");
		labeltext = labeltext + "5";
		"6" =>
		sys->print("6");
		labeltext = labeltext + "6";
		"7" =>
		sys->print("7");
		labeltext = labeltext + "7";
		"8" =>
		sys->print("8");
		labeltext = labeltext + "8";
		"9" =>
		sys->print("9");
		labeltext = labeltext + "9";
		"0" =>
		sys->print("0");
		labeltext = labeltext + "0";
		"back" =>
		if(labeltext != "") {
			labeltext = labeltext[:len labeltext - 1];
		}
		"dial" =>
		sys->print("dialing %s\n", labeltext);
		dial(labeltext);
		route(audio_route);
		get_call_list();
		"hangup" =>
		hangup_current();
		"switch_route" =>
		if(audio_route == EARPIECE) {
			route(SPEAKER);
			audio_route = SPEAKER;
			tk->cmd(t, ".b.switchroute configure -text Earpiece");
		} else {
			route(EARPIECE);
			audio_route = EARPIECE;
			tk->cmd(t, ".b.switchroute configure -text Speaker");
		}
		}
		tk->cmd(t, ".l configure -text {" + labeltext + "}");
		tk->cmd(t, "update");
	phonemsg := <- phonech =>
		case phonemsg {
		"ring" =>
			phone_state = RINGING;
			sys->print("ring!");
		}
		tk->cmd(t, ".l configure -text {" + phonemsg +"}");
		tk->cmd(t, "update");
	}
}

route(path : int)
{
	fd := sys->open("/phone/ctl", sys->OWRITE);
	if(fd == nil) {
		sys->fprint(sys->fildes(2), "could not open phone control device: %r\n");
		return;
	}
	if(path == EARPIECE) {
		sys->fprint(fd, "route earpiece");
	} else {
		sys->fprint(fd, "route speaker");
	}
}

set_volume(volume : real)
{
	fd := sys->open("/phone/ctl", sys->OWRITE);
	if(fd == nil) {
		sys->fprint(sys->fildes(2), "could not open phone control device: %r\n");
		return;
	}
	sys->fprint(fd, "volume %f", volume);
}

hangup(index : int)
{
	fd := sys->open("/phone/phone", sys->OWRITE);
	if(fd == nil) {
		sys->fprint(sys->fildes(2), "could not open phone: %r\n");
		return;
	}
	sys->fprint(fd, "hangup %d", index);
	phone_state = IDLE;
}

hangup_current()
{
	fd := sys->open("/phone/phone", sys->OWRITE);
	if(fd == nil) {
		sys->fprint(sys->fildes(2), "could not open phone: %r\n");
		return;
	}
	sys->fprint(fd, "hangup");
	phone_state = IDLE;
}

dial(number : string)
{
	fd := sys->open("/phone/phone", sys->OWRITE);
	if(fd == nil) {
		sys->fprint(sys->fildes(2), "could not open phone: %r\n");
		return;
	}
	sys->fprint(fd, "dial %s", number);
	phone_state = ACTIVE;
}

monitor_buttons()
{
	volume := 10;
	fd := sys->open("/dev/buttons", sys->OREAD);
	if(fd == nil) {
		sys->fprint(sys->fildes(2), "could not open buttons: %r\n");
		return;
	}
	for(;;) {
		newstr : string;
		buf := array[64] of byte;
		n := sys->read(fd, buf, len buf);
		buf = buf[:n];
		str := string buf;
		if(rstrip(str) == "volume up press") {
			if(volume < 10) {
				volume += 1;
				set_volume(real volume / 10.0);
			}
		} else if(rstrip(str) == "volume down press") {
			if(volume > 0) {
				volume -= 1;
				set_volume(real volume / 10.0);
			}
		}
	}
}

monitor(phonech : chan of string)
{
	fd := sys->open("/phone/phone", sys->OREAD);
	if(fd == nil) {
		sys->fprint(sys->fildes(2), "could not open phone: %r\n");
		return;
	}
	for(;;) {
		newstr : string;
		buf := array[64] of byte;
		n := sys->read(fd, buf, len buf);
		buf = buf[:n];
		str := string buf;
		phonech <-= rstrip(str);
	}
}

# Currently just throws away data. Only used to fix hangup bug.
get_call_list()
{
	buf := array[256] of byte;
	fd := sys->open("/phone/calls", sys->OREAD);
	if(fd == nil) {
		sys->fprint(sys->fildes(2), "could not open calls: %r\n");
		return;
	}
	n := sys->read(fd, buf, len buf);
}

strstr(s, t : string) : int
{
	if (t == nil)
		return 0;
	n := len t;
	if (n > len s)
		return -1;
	e := len s - n;
	for (p := 0; p <= e; p++)
		if (s[p:p+n] == t)
			return p;
	return -1;
}

rstrip(s : string) : string
{
	for(i := len s - 1; i >= 0; i--) {
		if(!str->in(s[i], "\r\n \t")) {
			return s[0:i + 1];
		}
	}
	return "";
}
