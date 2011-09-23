implement Toolbar;
include "sys.m";
	sys: Sys;
include "draw.m";
	draw: Draw;
	Screen, Display, Image, Rect, Point, Wmcontext, Pointer: import draw;
include "tk.m";
	tk: Tk;
include "tkclient.m";
	tkclient: Tkclient;
include "sh.m";
	shell: Sh;
	Listnode, Context: import shell;
include "string.m";
	str: String;
include "arg.m";

myselfbuiltin: Shellbuiltin;

Toolbar: module 
{
	init:	fn(ctxt: ref Draw->Context, argv: list of string);
	initbuiltin: fn(c: ref Context, sh: Sh): string;
	runbuiltin: fn(c: ref Context, sh: Sh,
			cmd: list of ref Listnode, last: int): string;
	runsbuiltin: fn(c: ref Context, sh: Sh,
			cmd: list of ref Listnode): list of ref Listnode;
	whatis: fn(c: ref Sh->Context, sh: Sh, name: string, wtype: int): string;
	getself: fn(): Shellbuiltin;
};

MAXCONSOLELINES:	con 1024;

tbtop: ref Tk->Toplevel;
screenr: Rect;
channel: ref Tk->Toplevel;
context : ref Draw->Context;
launched := 0;

whichmenu := 0;
# Name, command
menu := array[] of {
	array[] of { # folder 0 / default
	"Files", "wm/ftree",
	"Edit", "wm/edit",
	"Charon", "charon",
	"Manual", "wm/man",
	"Shell", "wm/sh",
	"Dialer", "wm/dialer",
	"SMS", "wm/sms",
	"Zygote", "startzygote",
	"->System", "folder:1",
	"->Misc", "folder:2",
	"->Games", "folder:3"
	}, array[] of { # folder 1
	"Debugger", "wm/deb",
	"Module manager", "wm/rt",
	"Task manager", "wm/task",
	"Memory monitor", "wm/memory",
	"About", "wm/about"
	}, array[] of { # folder2
	"Clock", "wm/date",
	"Colors", "wm/colors",
	"Coffee", "wm/coffee",
	"Polyhedra", "wm/polyhedra",
	"Fractals", "wm/mand",
	"Calendar", "wm/calendar",
	}, array[] of { # folder 3
	"Tetris", "wm/tetris",
	"Bounce", "wm/bounce",
	"Minesweeper", "wm/sweeper",
	"Connect 4", "wm/c4"
	}
};

badmodule(p: string)
{
	sys->fprint(sys->fildes(2), "toolbar: cannot load %s: %r\n", p);
	raise "fail:bad module";
}

init(ctxt: ref Draw->Context, argv: list of string)
{
	sys  = load Sys Sys->PATH;
	draw = load Draw Draw->PATH;
	if(draw == nil)
		badmodule(Draw->PATH);
	tk   = load Tk Tk->PATH;
	if(tk == nil)
		badmodule(Tk->PATH);

	str = load String String->PATH;
	if(str == nil)
		badmodule(String->PATH);

	tkclient = load Tkclient Tkclient->PATH;
	if(tkclient == nil)
		badmodule(Tkclient->PATH);
	tkclient->init();

	shell = load Sh Sh->PATH;
	if (shell == nil)
		badmodule(Sh->PATH);
	arg := load Arg Arg->PATH;
	if (arg == nil)
		badmodule(Arg->PATH);

	myselfbuiltin = load Shellbuiltin "$self";
	if (myselfbuiltin == nil)
		badmodule("$self(Shellbuiltin)");

	sys->pctl(Sys->NEWPGRP|Sys->FORKNS, nil);

	sys->bind("#p", "/prog", sys->MREPL);
	sys->bind("#s", "/chan", sys->MBEFORE);

	arg->init(argv);
	arg->setusage("toolbar [-s] [-p]");
#	ownsnarf := (sys->open("/chan/snarf", Sys->ORDWR) == nil);
	ownsnarf := sys->stat("/chan/snarf").t0 < 0;
	while((c := arg->opt()) != 0){
		case c {
		's' =>
			;
		'p' =>
			ownsnarf = 1;
		* =>
			arg->usage();
		}
	}
	argv = arg->argv();
	arg = nil;

	if (ctxt == nil){
		sys->fprint(sys->fildes(2), "toolbar: must run under a window manager\n");
		raise "fail:no wm";
	}

	exec := chan of string;
	launchchannel := chan of string;

	tbtop = toolbar(ctxt, exec, nil);
	tkclient->startinput(tbtop, "ptr" :: "control" :: nil);
	tkclient->onscreen(tbtop, "exact");
	
	shctxt := Context.new(ctxt);
	shctxt.addmodule("wm", myselfbuiltin);

	snarfIO: ref Sys->FileIO;
	if(ownsnarf){
		snarfIO = sys->file2chan("/chan", "snarf");
		if(snarfIO == nil)
			fatal(sys->sprint("cannot make /chan/snarf: %r"));
	}else
		snarfIO = ref Sys->FileIO(chan of (int, int, int, Sys->Rread), chan of (int, array of byte, int, Sys->Rwrite));
	sync := chan of string;
	spawn consoleproc(ctxt, sync);
	if ((err := <-sync) != nil)
		fatal(err);

	setupfinished := chan of int;
	donesetup := 0;
	spawn setup(shctxt, setupfinished);

	cmd := chan of string;

	snarf: array of byte;
#	write("/prog/"+string sys->pctl(0, nil)+"/ctl", "restricted"); # for testing
	for(;;) alt{
	k := <-tbtop.ctxt.kbd =>
		tk->keyboard(tbtop, k);
	m := <-tbtop.ctxt.ptr =>
		tk->pointer(tbtop, *m);
	s := <-tbtop.ctxt.ctl or
	s = <-tbtop.wreq =>
		wmctl(tbtop, s);
	s := <-exec =>
		# guard against parallel access to the shctxt environment
		if (s != "launcher") {
			sys->fprint(sys->fildes(2), "s = %s\n", s);
			if (donesetup){
				{
 					shctxt.run(ref Listnode(nil, s) :: nil, 0);
				} exception {
				"fail:*" =>	;
				}
			}
		}
		if (s == "launcher") {
			launcher(tbtop, launchchannel, whichmenu);
		}
	cmd := <-launchchannel =>
		launch(ctxt, cmd, tbtop, launchchannel);
	(off, data, nil, wc) := <-snarfIO.write =>
		if(wc == nil)
			break;
		if (off == 0)			# write at zero truncates
			snarf = data;
		else {
			if (off + len data > len snarf) {
				nsnarf := array[off + len data] of byte;
				nsnarf[0:] = snarf;
				snarf = nsnarf;
			}
			snarf[off:] = data;
		}
		wc <-= (len data, "");
	(off, nbytes, nil, rc) := <-snarfIO.read =>
		if(rc == nil)
			break;
		if (off >= len snarf) {
			rc <-= (nil, "");		# XXX alt
			break;
		}
		e := off + nbytes;
		if (e > len snarf)
			e = len snarf;
		rc <-= (snarf[off:e], "");	# XXX alt
	donesetup = <-setupfinished =>
		;	
	}
}

wmctl(top: ref Tk->Toplevel, c: string)
{
	args := str->unquoted(c);
	if(args == nil)
		return;

	case hd args{
	"request" =>
		# request clientid args...
	"newclient" =>
		# newclient id
		;
	"delclient" =>
		# delclient id
		;
	"rect" =>
		tkclient->wmctl(top, c);
		tkclient->onscreen(tbtop, "exact");
	* =>
		tkclient->wmctl(top, c);
	}
}

toolbar(ctxt: ref Draw->Context, exec, task: chan of string): ref Tk->Toplevel
{
	(tbtop, nil) = tkclient->toplevel(ctxt, nil, nil, Tkclient->Plain);
	screenr = tbtop.screenr;

	tk->namechan(tbtop, exec, "exec");

	cmd(tbtop, "frame .toolbar");
	cmd(tbtop, "button .toolbar.start -borderwidth 0 -bitmap vitasmall.bit -width " + string screenr.dx() + " -height 64 -command {send exec launcher}");
	cmd(tbtop, "pack .toolbar.start -side left -in .toolbar");
	cmd(tbtop, "pack .toolbar -fill x -in .");

	return tbtop;
}

launcher(tbtop:ref Tk->Toplevel, launchchannel: chan of string, whichmenu: int)
{
	if(launched == 1) {
		cmd(tbtop, "destroy .launcher");
		launched = 0;
		cmd(tbtop, ".toolbar configure -borderwidth 0");
		cmd(tbtop, "update");
		return;
	}

	tk->namechan(tbtop, launchchannel, "channel");

	cmd(tbtop, ".toolbar.start configure -borderwidth 1");

	cmd(tbtop, "frame .launcher");
	for(i := 0; i * 4 < len menu[whichmenu]; i++) {
		j := (i * 4);
		cmd(tbtop, sys->sprint("frame .launcher.line%d", i));
		if (j + 2 == len menu[whichmenu]) {
			cmd(tbtop, sys->sprint("button .launcher.line%d.b1 -borderwidth 1 -text %s -height 64 -width %d -command {send channel %s}", i, menu[whichmenu][j], screenr.dx(), menu[whichmenu][j + 1]));
			cmd(tbtop, sys->sprint("pack .launcher.line%d.b1 -side left -in .launcher.line%d", i, i));
		} else {
			cmd(tbtop, sys->sprint("button .launcher.line%d.b1 -borderwidth 1 -text %s -height 64 -width %d -command {send channel %s}", i, menu[whichmenu][j], (screenr.dx() / 2), menu[whichmenu][j + 1]));
			cmd(tbtop, sys->sprint("button .launcher.line%d.b2 -borderwidth 1 -text %s -height 64 -width %d -command {send channel %s}", i, menu[whichmenu][j + 2], (screenr.dx() / 2), menu[whichmenu][j + 3]));
			cmd(tbtop, sys->sprint("pack .launcher.line%d.b1 .launcher.line%d.b2 -side left -in .launcher.line%d", i, i, i));
		}
		cmd(tbtop, sys->sprint("pack .launcher.line%d -fill x -in .launcher", i));
	}
	cmd(tbtop, "pack .launcher -fill x -in .");
	cmd(tbtop, "update");

	launched = 1;
}

launch(ctxt: ref Draw->Context, appl: string, tbtop: ref Tk->Toplevel, launchchannel: chan of string)
{
	if (len appl == 8 && appl[:7] == "folder:") {
		whichmenu = int sys->sprint("%c", appl[7]);
		launcher(tbtop, launchchannel, whichmenu);
		launcher(tbtop, launchchannel, whichmenu);
		whichmenu = 0;
		return;
	}
	tkclient->wmctl(tbtop, sys->sprint("newwin %s", appl));
	sh := load Sh Sh->PATH;
	argv := appl :: nil;
	sh->run(ctxt, "{$*&}" :: argv);

	launcher(tbtop, launchchannel, whichmenu);
}

setup(shctxt: ref Context, finished: chan of int)
{
	ctxt := shctxt.copy(0);
	ctxt.run(shell->stringlist2list("run"::"/lib/wmsetup"::nil), 0);
	cmd(tbtop, "update");
	finished <-= 1;
}

condenselabel(label: string): string
{
	if(len label > 15){
		new := "";
		l := 0;
		while(len label > 15 && l < 3) {
			new += label[0:15]+"\n";
			label = label[15:];
			for(v := 0; v < len label; v++)
				if(label[v] != ' ')
					break;
			label = label[v:];
			l++;
		}
		label = new + label;
	}
	return label;
}

initbuiltin(ctxt: ref Context, nil: Sh): string
{
	if (tbtop == nil) {
		sys = load Sys Sys->PATH;
		sys->fprint(sys->fildes(2), "wm: cannot load wm as a builtin\n");
		raise "fail:usage";
	}
	ctxt.addbuiltin("error", myselfbuiltin);
	return nil;
}

whatis(nil: ref Sh->Context, nil: Sh, nil: string, nil: int): string
{
	return nil;
}

runbuiltin(c: ref Context, sh: Sh,
			cmd: list of ref Listnode, nil: int): string
{
	return nil;
}

runsbuiltin(nil: ref Context, nil: Sh,
			nil: list of ref Listnode): list of ref Listnode
{
	return nil;
}

getself(): Shellbuiltin
{
	return myselfbuiltin;
}

cmd(top: ref Tk->Toplevel, c: string): string
{
	s := tk->cmd(top, c);
	if (s != nil && s[0] == '!')
		sys->fprint(sys->fildes(2), "tk error on %#q: %s\n", c, s);
	return s;
}

kill(pid: int, note: string): int
{
	fd := sys->open("/prog/"+string pid+"/ctl", Sys->OWRITE);
	if(fd == nil || sys->fprint(fd, "%s", note) < 0)
		return -1;
	return 0;
}

fatal(s: string)
{
	sys->fprint(sys->fildes(2), "wm: %s\n", s);
	kill(sys->pctl(0, nil), "killgrp");
	raise "fail:error";
}

con_cfg := array[] of
{
	"frame .cons",
	"scrollbar .cons.scroll -command {.cons.t yview}",
	"text .cons.t -width 60w -height 15w -bg white "+
		"-fg black -font /fonts/misc/latin1.6x13.font "+
		"-yscrollcommand {.cons.scroll set}",
	"pack .cons.scroll -side left -fill y",
	"pack .cons.t -fill both -expand 1",
	"pack .cons -expand 1 -fill both",
	"pack propagate . 0",
	"update"
};
nlines := 0;		# transcript length

consoleproc(ctxt: ref Draw->Context, sync: chan of string)
{
	iostdout := sys->file2chan("/chan", "wmstdout");
	if(iostdout == nil){
		sync <-= sys->sprint("cannot make /chan/wmstdout: %r");
		return;
	}
	iostderr := sys->file2chan("/chan", "wmstderr");
	if(iostderr == nil){
		sync <-= sys->sprint("cannot make /chan/wmstdout: %r");
		return;
	}

	sync <-= nil;

	(top, titlectl) := tkclient->toplevel(ctxt, "", "Log", tkclient->Appl); 
	for(i := 0; i < len con_cfg; i++)
		cmd(top, con_cfg[i]);

	r := tk->rect(top, ".", Tk->Border|Tk->Required);
	cmd(top, ". configure -x " + string ((top.screenr.dx() - r.dx()) / 2 + top.screenr.min.x) +
				" -y " + string (r.dy() / 3 + top.screenr.min.y));

	tkclient->startinput(top, "ptr"::"kbd"::nil);
	tkclient->onscreen(top, "onscreen");
	tkclient->wmctl(top, "task");

	for(;;) alt {
	c := <-titlectl or
	c = <-top.wreq or
	c = <-top.ctxt.ctl =>
		if(c == "exit")
			c = "task";
		tkclient->wmctl(top, c);
	c := <-top.ctxt.kbd =>
		tk->keyboard(top, c);
	p := <-top.ctxt.ptr =>
		tk->pointer(top, *p);
	(nil, nil, nil, rc) := <-iostdout.read =>
		if(rc != nil)
			rc <-= (nil, "inappropriate use of file");
	(nil, nil, nil, rc) := <-iostderr.read =>
		if(rc != nil)
			rc <-= (nil, "inappropriate use of file");
	(nil, data, nil, wc) := <-iostdout.write =>
		conout(top, data, wc);
	(nil, data, nil, wc) := <-iostderr.write =>
		conout(top, data, wc);
		if(wc != nil)
			tkclient->wmctl(top, "untask");
	}
}

conout(top: ref Tk->Toplevel, data: array of byte, wc: Sys->Rwrite)
{
	if(wc == nil)
		return;

	s := string data;
	tk->cmd(top, ".cons.t insert end '"+ s);
	alt{
	wc <-= (len data, nil) =>;
	* =>;
	}

	for(i := 0; i < len s; i++)
		if(s[i] == '\n')
			nlines++;
	if(nlines > MAXCONSOLELINES){
		cmd(top, ".cons.t delete 1.0 " + string (nlines/4) + ".0; update");
		nlines -= nlines / 4;
	}

	tk->cmd(top, ".cons.t see end; update");
}
