implement Sms;

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
	"frame .thread",
	"text .messages -state disabled -wrap word -font /fonts/lucida/unicode.14.font -yscrollcommand {.sb set}",
	"scrollbar .sb -width 40 -orient vertical -command {.messages yview}",
	"pack .sb -in .thread -side left -fill y",
	"pack .messages -in .thread -side left -fill both -expand 1",
	"pack .thread -side top -anchor center -fill both -expand 1",
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

	#tk->cmd(t, ".f.lb insert end 'john");
	convs := loadconvos();
	for (; convs != nil; convs = tl convs)
		tk->cmd(t, ".f.lb insert end '"+ hd convs);

	tkclient->onscreen(t, nil);
	tkclient->startinput(t, "ptr"::nil);

	u : string;	
	# Event loop
	for(;;) {
		alt {
		s := <-t.ctxt.kbd =>
			tk->keyboard(t, s);
		s := <-t.ctxt.ptr =>
			tk->pointer(t, *s);
		s := <-t.ctxt.ctl or
		s = <-t.wreq or
		s = <-wmchan =>
			tkclient->wmctl(t, s);
		}
		sel := tk->cmd(t, ".f.lb curselection");
		if (sel == nil)
			continue;
		u = tk->cmd(t, ".f.lb get " + sel);
		sys->print("read %s\n", u);
		if (u != nil)
			break;
	}

	# Clear da screen
	tk->cmd(t, "pack forget .conv");

	for (i = 0; i < len thread_window; i++)
		tk->cmd(t, thread_window[i]);

	tk->cmd(t, "update");

	data := bio->open("/usr/"+rf("/dev/user")+"/lib/sms/"+u, bio->OREAD);
	if (data == nil) {
		sys->fprint(sys->fildes(2), "can't open file\n");
		return;
	}

	#flipflop := 0;
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

			#flipflop = !flipflop;
		}
	}

	tk->cmd(t, ".messages scan mark 0 0");
	tk->cmd(t, ".messages scan dragto -100000 -100000");
	tk->cmd(t, "update");

	# TODO: take this out after we do the appropriate looping crapp
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
