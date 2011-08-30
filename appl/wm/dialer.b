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
	"pack .l .b.back",
	"pack .row1 .row2 .row3 .row4 -fill both -in .b",
	"pack .b.dial .b.hangup -fill both -in .b",
	"pack .b -fill both",
	"pack .fl -fill both -expand 1",
	"pack propagate . 0",
	"update",
};

init(ctxt: ref Draw->Context, nil: list of string)
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
		"hangup" =>
		hangup(1);
		}
		tk->cmd(t, ".l configure -text {" + labeltext + "}");
		tk->cmd(t, "update");
	phonemsg := <- phonech =>
		case phonemsg {
		"ring" =>
			sys->print("ring!");
		}
		tk->cmd(t, ".l configure -text {" + phonemsg +"}");
		tk->cmd(t, "update");
	}
}

hangup(index : int)
{
	fd := sys->open("/phone/phone", sys->OWRITE);
	if(fd == nil) {
		sys->fprint(sys->fildes(2), "could not open phone: %r\n");
		return;
	}
	str := sys->sprint("hangup %d", index);
	sys->write(fd, array of byte str, len (array of byte str));
}

dial(number : string)
{
	fd := sys->open("/phone/phone", sys->OWRITE);
	if(fd == nil) {
		sys->fprint(sys->fildes(2), "could not open phone: %r\n");
		return;
	}
	str := sys->sprint("dial %s", number);
	sys->write(fd, array of byte str, len (array of byte str));	
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
