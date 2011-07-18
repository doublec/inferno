implement ButtonServer;

include "sys.m";
sys : Sys;
include "draw.m";
include "styx.m";
styx: Styx;
Rmsg, Tmsg: import styx;
include "styxservers.m";
styxservers: Styxservers;
Styxserver, Navigator: import styxservers;
nametree: Nametree;
Tree: import nametree;
include "cfg.m";
cfg: Cfg;
include "arg.m";
arg : Arg;

cfgfile := "/etc/buttonserver.cfg";
mntflg := Sys->MBEFORE;
mntpt := "/dev";
Qroot, Qbuttons : con big iota;

ButtonServer: module {
	init: fn(nil: ref Draw->Context, args: list of string);
};

Reader: adt {
	msgs: chan of ref Tmsg.Read;
	fid: ref Styxservers->Fid;
	data: chan of string;
};

MAX_READERS : con 1024;

readers := array [MAX_READERS] of Reader;

init(nil: ref Draw->Context, args: list of string)
{
	sys = load Sys Sys->PATH;
	styx = load Styx Styx->PATH;
	styx->init();
	styxservers = load Styxservers Styxservers->PATH;
	styxservers->init(styx);
	nametree = load Nametree Nametree->PATH;
	nametree->init();
	arg = load Arg Arg->PATH;
	arg->init(args);
	while((c := arg->opt()) != 0) {
		case c {
		'c' => cfgfile = arg->arg();
		* => sys->print("unrecognized option %c\n", c);
		}
	}
	cfg = load Cfg Cfg->PATH;
	if(cfg->init(cfgfile) != "") {
		raise sys->sprint("could not access configuration file %s", cfgfile);
	}

	for(i := 0; i < MAX_READERS; i++) {
		readers[i].fid = nil;
	}

	spawn read_events();

	styxpipe := array [2] of ref Sys->FD;
	sys->pipe(styxpipe);
	(tree, treeop) := nametree->start();
	tree.create(Qroot, dir(".", 8r555|Sys->DMDIR, Qroot));
	tree.create(Qroot, dir("buttons", 8r666, Qbuttons));
	(tchan, srv) := Styxserver.new(styxpipe[0], Navigator.new(treeop), Qroot);
	# sys->mount will never return if we call it directly here
	spawn mounter(styxpipe[1]);

	while((gm := <- tchan) != nil) {
		pick m := gm {
			Read =>
				(fid, err) := srv.canread(m);
				if(fid == nil) {
					sys->print("error getting fid: %s\n", err);
				}
				if(fid.path == Qbuttons) {
					reader := get_reader(fid, srv);
					deliver_msg(reader, m);
				} else {
					srv.default(gm);
				}
			Clunk =>
				fid := srv.clunk(m);
				if(fid != nil && fid.path == Qbuttons) {
					free_reader(fid);
				}
								
			* =>		
				srv.default(gm);
		}
	}
	tree.quit();
}

read_events()
{
	fd := sys->open("/dev/events", sys->OREAD);
	for(;;) {
		newstr : string;
		buf := array[64] of byte;
		n := sys->read(fd, buf, len buf);
		buf = buf[:n];
		str := string buf;
		for(i := 0; i < MAX_READERS; i++) {
			if(readers[i].fid != nil) {
				reader := readers[i];
				reader.data <-= str;
			}
		}
	}
}

mounter(styxfd : ref Sys->FD)
{
	if(sys->mount(styxfd, nil, mntpt, mntflg, nil) < 0) {
		sys->print("fail: ButtonServer mount failed: %r\n");
	}
}

dir(name: string, perm: int, qid: big): Sys->Dir
{
	d := sys->zerodir;
	d.name = name;
	d.uid = "me";
	d.gid = "me";
	d.qid.path = qid;
	if (perm & Sys->DMDIR)
		d.qid.qtype = Sys->QTDIR;
	else
		d.qid.qtype = Sys->QTFILE;
	d.mode = perm;
	return d;
}

get_reader(fid: ref Styxservers->Fid, srv: ref Styxserver) : Reader
{
	for(i := 0; i < MAX_READERS; i++) {
		if(readers[i].fid == fid) {
			return readers[i];
		}
	}

	# reader was not found, create new entry
	for(i = 0; i < MAX_READERS; i++) {
		if(readers[i].fid == nil) {
			readers[i].fid = fid;
			readers[i].msgs = chan of ref Tmsg.Read;
			readers[i].data = chan of string;
			spawn read_msgs(readers[i], srv);
			return readers[i];
		}
	}
	raise(sys->sprint("not enough space for new reader"));
	return readers[0];
}

deliver_msg(reader: Reader, msg: ref Tmsg.Read)
{
	reader.msgs <-= msg;
}

read_msgs(reader: Reader, srv: ref Styxserver)
{
	for(;;) {
		str: string;
		msg := <- reader.msgs;
		while(1) {
			str = <- reader.data;
			str = interpret(str);
			if(str != "") {
				reply := ref Rmsg.Read(msg.tag, nil);
				reply.data = array of byte str;
				srv.reply(reply);
				break;
			}
		}
	}
}

interpret(event : string) : string
{
	str: string;
	a: ref Cfg->Record;
	lst := cfg->lookup(event[:len event - 1]);
	if(lst == nil) return "";
	(str, a) = hd lst;
	if(str == nil) str = "";
	return str + "\n";
}

free_reader(fid: ref Styxservers->Fid)
{
	for(i := 0; i < MAX_READERS; i++) {
		if(readers[i].fid == fid) {
			readers[i].fid = nil;
		}
	}
}