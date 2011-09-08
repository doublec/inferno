implement Sms;

# This module is amateur hour.
# Author: John Floren

include "sys.m";
	sys: Sys;

include "draw.m";
	draw: Draw;
	Display: import draw;

include "tk.m";
	tk: Tk;

include "tkclient.m";
	tkclient: Tkclient;

include "bufio.m";
	bio: Bufio;

include "readdir.m";

Sms: module
{
	init: fn(ctxt: ref Draw->Context, nil: list of string);
};

# display all the conversations
conv_window := array[] of {
	"frame .conv",
	"frame .f",
	"listbox .f.lb -font /fonts/lucida/unicode.18.font -yscrollcommand {.f.sb set}",
	"scrollbar .f.sb -width 40 -orient vertical -command {.f.lb yview}",
	"pack .f.sb -in .f -side left -fill y",
	"pack .f.lb -in .f -side left -fill both -expand 1",
	"pack .f -in .conv -side top -anchor center -fill both -expand 1",
	"pack .conv -side top -anchor center -fill both -expand 1",
	"pack propagate . 0",
};

# display the conversation with just one person
thread_window := array[] of {
	"frame .hist",
	"frame .compose",
	"text .messages -state disabled -wrap word -font /fonts/lucida/unicode.14.font -yscrollcommand {.sb set}",
	"scrollbar .sb -width 40 -orient vertical -command {.messages yview}",
	"pack .sb -in .hist -side left -fill y",
	"pack .messages -in .hist -side left -fill both -expand 1",
	#"pack .hist -side top -anchor center -fill both -expand 1",
	"text .compose.t -height 3 -wrap word -font /fonts/lucida/unicode.14.font -yscrollcommand {.compose.sb set}",
	"scrollbar .compose.sb -width 40 -orient vertical -command {.compose.t yview}",
	"button .compose.send -text {Send} -font /fonts/lucida/unicode.12.font -command {send composecmd sendsms}",
	"pack .compose.sb -in .compose -side left -fill y",
	"pack .compose.send -in .compose -side right -fill y -ipadx 3 -expand 1",
	"pack .compose.t -in .compose -side left -fill y",
	"button .goback -text {Back} -font /fonts/lucida/unicode.12.font -command {send composecmd goback}",
	"pack .goback -side top -anchor nw",
	"pack .hist -side top -fill both -expand 1",
	"pack .compose -side top -fill both -expand 1",
	".pack propagate . 0",
};

init(ctxt: ref Draw->Context, nil: list of string)
{
	sys = load Sys Sys->PATH;
	if (ctxt == nil) {
		sys->fprint(sys->fildes(2), "about: no window context\n");
		raise "fail:bad context";
	}

	draw = load Draw Draw->PATH;
	tk   = load Tk   Tk->PATH;
	tkclient= load Tkclient Tkclient->PATH;
	bio = load Bufio Bufio->PATH;

	sys->pctl(Sys->NEWPGRP, nil);

	tkclient->init();
	(t, wmchan) := tkclient->toplevel(ctxt, nil, "SMS", 0);

	for (i := 0; i < len conv_window; i++)
		tk->cmd(t, conv_window[i]);

	convs := loadconvos();
	for (; convs != nil; convs = tl convs)
		tk->cmd(t, ".f.lb insert end '"+ hd convs);

	tkclient->onscreen(t, nil);
	tkclient->startinput(t, "ptr"::nil);

	u : string;	
	# Event loop
	for(;;) {
		tk->cmd(t, "update");
		alt {
		s := <-t.ctxt.kbd =>
			tk->keyboard(t, s);
		s := <-t.ctxt.ptr =>
			tk->pointer(t, *s);
			if (s.buttons == 0) {
			sel := tk->cmd(t, ".f.lb curselection");
			if (sel == nil)
				continue;
			u = tk->cmd(t, ".f.lb get " + sel);
			sys->print("read %s\n", u);
			if (u != nil) {
				#break;
				spawn readthread(ctxt, u);
			}
			tk->cmd(t, ".f.lb selection clear 0");
			}
		s := <-t.ctxt.ctl or
		s = <-t.wreq or
		s = <-wmchan =>
			tkclient->wmctl(t, s);
		}

	}
}

# We spawn a new window so we could, theoretically, minimize this
# and go open a new thread to read at the same time.
readthread(ctxt: ref Draw->Context, u: string)
{
	(t, wmchan) := tkclient->toplevel(ctxt, nil, "Send message", 0);
	cmd := chan of string;
	tk->namechan(t, cmd, "cmd");

	# We need a channel for commands and such
	composecmd := chan of string;
	tk->namechan(t, composecmd, "composecmd");

	for (i := 0; i < len thread_window; i++)
		tk->cmd(t, thread_window[i]);

	tk->cmd(t, "update");

	tkclient->onscreen(t, "onscreen");

	data := bio->open("/usr/"+rf("/dev/user")+"/lib/sms/"+u, bio->OREAD);
	if (data == nil) {
		sys->fprint(sys->fildes(2), "can't open file\n");
		return;
	}

	tk->cmd(t, ".messages tag configure me -fg black");
	tk->cmd(t, ".messages tag configure them -fg #3333ee");
	tk->cmd(t, "update");
	while ((s := bio->data.gets('\n')) != nil) {
		if (len s > 0) {
			if (s[:3] == "Me:")
				tk->cmd(t, ".messages insert end " + tk->quote(s) + " { me }");
			else
				tk->cmd(t, ".messages insert end " + tk->quote(s) + " { them }");

			tk->cmd(t, ".messages insert end '\n");
		}
	}

	tk->cmd(t, ".messages scan mark 0 0");
	tk->cmd(t, ".messages scan dragto -100000 -100000");
	tk->cmd(t, "update");

	tkclient->startinput(t, "kbd"::"ptr"::nil);
	pid := chan of int;
	spawn updater(t, data, pid);
	updaterpid := <-pid;
	for (;;) {
		alt {
		s := <-t.ctxt.kbd =>
			tk->keyboard(t, s);
		s := <-t.ctxt.ptr =>
			tk->pointer(t, *s);
		s := <-t.ctxt.ctl or
		s = <-t.wreq or
		s = <-wmchan =>
				tkclient->wmctl(t, s);
		s := <-composecmd =>
			sys->print("read %s\n", s);
			if (s == "goback") {
				kill(updaterpid);
				return;
			} else if (s == "sendsms") {
				txt := tk->cmd(t, ".compose.t get 1.0 end");
				sys->print("to send: %s\n", txt);

				# open the SMS file
				sendfile := bio->open("/phone/sms", bio->OWRITE);
				if (sendfile != nil) {
					bio->sendfile.puts("send " + u + " " + txt);
					bio->sendfile.close();
				} else {
					sys->fprint(sys->fildes(2), "can't open /phone/sms\n");
				}
			}
		}
	}
}

updater(t: ref Tk->Toplevel, data: ref Bufio->Iobuf, pid: chan of int)
{
	pid <- = sys->pctl(0, nil);
	for (;;) {
		s := bio->data.gets('\n');
		if (len s > 0) {
			if (s[:3] == "Me:")
				tk->cmd(t, ".messages insert end " + tk->quote(s) + " { me }");
			else
				tk->cmd(t, ".messages insert end " + tk->quote(s) + " { them }");

			tk->cmd(t, ".messages insert end '\n");
			tk->cmd(t, ".messages scan mark 0 0");
			tk->cmd(t, ".messages scan dragto -100000 -100000");
			tk->cmd(t, "update");
			sys->sleep(1000);
		}
	}
}


# Get a list of files under /usr/$user/lib/sms/
loadconvos(): list of string
{
	readdir := load Readdir Readdir->PATH;
	if (readdir == nil)
		return nil;

	(convs, nil) := readdir->init("/usr/" + rf("/dev/user") + "/lib/sms", Readdir->MTIME);
	n: list of string;
	for (i := len convs - 1; i >= 0; i--)
		if (! (convs[i].qid.qtype & Sys->QTDIR))
			n = convs[i].name :: n;

	return n;
}

rf(file: string): string
{
	fd := sys->open(file, sys->OREAD);
	if(fd == nil)
		return "";

	buf := array[128] of byte;
	n := sys->read(fd, buf, len buf);
	if(n < 0)
		return "";

	return string buf[0:n];	
}

kill(pid: int)
{
	fd := sys->open("#p/"+string pid+"/ctl", Sys->OWRITE);
	if(fd != nil)
		sys->fprint(fd, "kill");
}
