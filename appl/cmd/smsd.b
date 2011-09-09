implement SmsDaemon;

include "sys.m";
	sys: Sys;

include "bufio.m";
	bio: Bufio;

include "draw.m";

include "regex.m";


SmsDaemon: module {
	init: fn(nil: ref Draw->Context, args: list of string);
};

init(nil: ref Draw->Context, argv: list of string)
{
	sys = load Sys Sys->PATH;
	bio = load Bufio Bufio->PATH;
	regex := load Regex Regex->PATH;

	# the directory where we'll store messages
	msgdir := "/usr/"+rf("/dev/user")+"/lib/sms/";

	# Open the sms interface
	data := bio->open("/phone/sms", bio->OREAD);
	#data := bio->fopen(sys->fildes(0), bio->OREAD);  # for testing
	if (data == nil) {
		sys->fprint(sys->fildes(2), "can't open file\n");
		return;
	}

	# This matches the current format of incoming SMS
	(re, nil) := regex->compile("([0-9]+) (\\(.*\\)): (.*)", 1);
	while ((s := bio->data.gets('\n')) != nil) {
		sys->print("Got raw message: %s", s);
		a := regex->execute(re, s);
		if (len(a) == 4) {
			# Grab the sender's number
			(beg, end) := a[1];
			sender := s[beg:end];
			#sys->print("sender = '%s'\n", sender);
			# Grab the rest of the message
			(beg, end) = a[2];
			timestamp := s[beg:];
	
			# Open the appropriate file
			filename := msgdir + sender;
			#sys->print("filename = %s\n", filename);
			# check existence of file
			(exists, nil) := sys->stat(filename);
			#sys->print("return from stat = %d\n", exists);
			out : ref bio->Iobuf;
			if (exists == 0)
				out = bio->open(filename, bio->OWRITE);
			else
				out = bio->create(filename, bio->OWRITE, 8r664);
	
			bio->out.seek(big 0, Sys->SEEKEND);
			if (out == nil) {
				sys->fprint(sys->fildes(2), "can't open file\n");
				return;
			}
	
			# Write our message to the file
			bio->out.puts(s);
	
			# Close it down
			bio->out.close();
		}
	}
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
