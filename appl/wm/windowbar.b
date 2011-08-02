implement Windowbar;
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

Windowbar: module 
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
	sys->fprint(sys->fildes(2), "windowbar: cannot load %s: %r\n", p);
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
	arg->setusage("windowbar [-s] [-p]");
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
		sys->fprint(sys->fildes(2), "windowbar: must run under a window manager\n");
		raise "fail:no wm";
	}

	exec := chan of string;
	task := chan of string;

	tbtop = windowbar(ctxt, exec, task);
	tkclient->startinput(tbtop, "ptr" :: "control" :: nil);
	layout(tbtop);
	spawn updatebattery(ctxt, tbtop, 1);

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
		wmctl(tbtop, s);
	s := <-exec =>
		# guard against parallel access to the shctxt environment
		if (s != "batterystatus") {
			if (donesetup){
				{
 					shctxt.run(ref Listnode(nil, s) :: nil, 0);
				} exception {
				"fail:*" =>	;
				}
			}
		}
		if (s == "batterystatus")
			updatebattery(ctxt, tbtop, 0);
	detask := <-task =>
		deiconify(detask);
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
	n := len args;

	case hd args{
	"request" =>
		# request clientid args...
		if(n < 3)
			return;
		args = tl args;
		clientid := hd args;
		args = tl args;
		err := handlerequest(clientid, args);
		if(err != nil)
			sys->fprint(sys->fildes(2), "windowbar: bad wmctl request %#q: %s\n", c, err);
	"newclient" =>
		# newclient id
		;
	"delclient" =>
		# delclient id
		deiconify(hd tl args);
	"rect" =>
		tkclient->wmctl(top, c);
		layout(top);
	* =>
		tkclient->wmctl(top, c);
	}
}

handlerequest(clientid: string, args: list of string): string
{
	n := len args;
	case hd args {
	"task" =>
		# task name
		if(n != 2)
			return "no task label given";
		iconify(clientid, hd tl args);
	"untask" or
	"unhide" =>
		deiconify(clientid);
	* =>
		return "unknown request";
	}
	return nil;
}

iconify(id, label: string)
{
	label = condenselabel(label);
	e := tk->cmd(tbtop, "button .windowbar." +id+" -command {send task "+id+"} -takefocus 0");
	cmd(tbtop, ".windowbar." +id+" configure -text '" + label);
	if(e[0] != '!')
		cmd(tbtop, "pack .windowbar."+id+" -side left -fill y");
	cmd(tbtop, "update");
}

deiconify(id: string)
{
	e := tk->cmd(tbtop, "destroy .windowbar."+id);
	if(e == nil){
		tkclient->wmctl(tbtop, sys->sprint("ctl %q untask", id));
		tkclient->wmctl(tbtop, sys->sprint("ctl %q kbdfocus 1", id));
	}
	cmd(tbtop, "update");
}

layout(top: ref Tk->Toplevel)
{
	r := top.screenr;
	h := 64;
	if(r.dy() < 480)
		h = tk->rect(top, ".b", Tk->Border|Tk->Required).dy();
	cmd(top, ". configure -x " + string r.min.x +
			" -y " + string (r.max.y - h) +
			" -width " + string r.dx() +
			" -height " + string h);
	cmd(top, "update");
	tkclient->onscreen(tbtop, "exact");
}

windowbar(ctxt: ref Draw->Context, exec, task: chan of string): ref Tk->Toplevel
{
	(tbtop, nil) = tkclient->toplevel(ctxt, nil, nil, Tkclient->Plain);
	screenr = tbtop.screenr;

	cmd(tbtop, "button .b -text {XXX}");
	cmd(tbtop, "pack propagate . 0");

	tk->namechan(tbtop, exec, "exec");
	tk->namechan(tbtop, task, "task");
	cmd(tbtop, "frame .windowbar");
	batterylevel : ref Sys->FD;
	batterylevel = sys->open("/dev/battery", sys->OREAD);
	battery := array[5] of byte;
	length := sys->read(batterylevel, battery, len battery);
	battery = battery[:length];
	level := string battery;
	cmd(tbtop, "button .windowbar.battery -text " + string level + "% -borderwidth 0 -width 64 -height 64 -command {send exec batterystatus}");
	cmd(tbtop, "pack .windowbar.battery -side left");
	cmd(tbtop, "pack .windowbar -fill x");
	return tbtop;
}

updatebattery(ctxt: ref Draw->Context, tbtop: ref Tk->Toplevel, wait: int)
{
	batterylevel : ref Sys->FD;
	batterylevel = sys->open("/dev/battery", sys->OREAD);
	battery := array[5] of byte;
	length := sys->read(batterylevel, battery, len battery);
	battery = battery[:length];
	level := string battery;
	cmd(tbtop, ".windowbar.battery configure -text " + string level + "%");
	cmd(tbtop, "update");
	if (wait == 1) {
		sys = load Sys Sys->PATH;
		sys->sleep(60000);
		updatebattery(ctxt, tbtop, 1);
	}
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

consoleproc(ctxt: ref Draw->Context, sync: chan of string)
{
	sync <-= nil;
}
