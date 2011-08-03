implement Brightness;
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

Brightness: module 
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

badmodule(p: string)
{
	sys->fprint(sys->fildes(2), "brightness: cannot load %s: %r\n", p);
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
	arg->setusage("brightness [-s] [-p]");
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
		sys->fprint(sys->fildes(2), "brightness: must run under a window manager\n");
		raise "fail:no wm";
	}

	exec := chan of string;

	tbtop = brightness(ctxt, exec);
	tkclient->startinput(tbtop, "ptr" :: "control" :: nil);
	layout(tbtop);

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

	snarf: array of byte;
#	write("/prog/"+string sys->pctl(0, nil)+"/ctl", "restricted"); # for testing
	for(;;) alt{
	k := <-tbtop.ctxt.kbd =>
		tk->keyboard(tbtop, k);
	m := <-tbtop.ctxt.ptr =>
		tk->pointer(tbtop, *m);
	s := <-tbtop.ctxt.ctl or
	s = <-tbtop.wreq =>
		if(len s >= 8) {
			if(s[:8] == "!reshape")
				break;
			if(s[:8] == "delete .")
				s = "exit";
		}
		if(s == "task")
			s = "exit";
		wmctl(tbtop, s);
	s := <-exec =>
		(nil, output) := sys->tokenize(s, " ");
		setbrightness(hd tl output);
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
	"newclient" =>
		# newclient id
		;
	"delclient" =>
		# delclient id
		;
	"rect" =>
		tkclient->wmctl(top, c);
		layout(top);
	* =>
		tkclient->wmctl(top, c);
	}
}

layout(top: ref Tk->Toplevel)
{
	r := top.screenr;
	h := 56;
	if(r.dy() < 480)
		h = tk->rect(top, ".b", Tk->Border|Tk->Required).dy();
	cmd(top, ". configure -x " + string r.min.x +
			" -y " + string (((r.max.y - r.min.y) / 2) - (h / 2)) +
			" -width " + string r.dx() +
			" -height " + string h);
	cmd(top, "update");
	tkclient->onscreen(tbtop, "exact");
}

brightness(ctxt: ref Draw->Context, exec : chan of string): ref Tk->Toplevel
{
	(tbtop, nil) = tkclient->toplevel(ctxt, nil, nil, Tkclient->Plain);
	screenr = tbtop.screenr;

	tk->namechan(tbtop, exec, "exec");
	cmd(tbtop, "frame .brightness");
	brightness : ref Sys->FD;
	brightness = sys->open("/dev/brightness", sys->OREAD);
	brightnesslevel := array[5] of byte;
	length := sys->read(brightness, brightnesslevel, len brightnesslevel);
	brightnesslevel = brightnesslevel[:length];
	level := string brightnesslevel;
	cmd(tbtop, "scale .brightness.level -height 56 -width " + string screenr.dx() + " -from 5 -to 255 -orient horizontal -showvalue 1 -command {send exec updatebrightness}");
	cmd(tbtop, ".brightness.level set " + level);
	cmd(tbtop, "pack .brightness.level -side left");
	cmd(tbtop, "pack .brightness -fill x");
	return tbtop;
}

setbrightness(howmuch : string)
{
	brightnesslevel := array[5] of byte;
	brightnesslevel = array of byte howmuch;
	brightness : ref Sys->FD;
	brightness = sys->open("/dev/brightness", sys->OWRITE);
	sys->write(brightness, brightnesslevel, len brightnesslevel);
}

setup(shctxt: ref Context, finished: chan of int)
{
	ctxt := shctxt.copy(0);
	ctxt.run(shell->stringlist2list("run"::"/lib/wmsetup"::nil), 0);
	cmd(tbtop, "update");
	finished <-= 1;
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

consoleproc(ctxt: ref Draw->Context, sync: chan of string)
{
	sync <-= nil;
}
