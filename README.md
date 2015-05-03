# Inferno 4th Edition

This is an unofficial conversion of the [Inferno OS](http://inferno-os.org) mercurial repository to
git performed by the [hg-git](http://hg-git.github.io/) mercurial plugin.

The Inferno website describes Inferno as a compact operating system for building cross
platform distributed systems. It features:

* Available as Free Software or with conventional commercial licence
* Runs as a full OS or an application
* Full development toolkit included
* Advanced networking capabilities
* Safe, concurrent programming environment
* Architecture independent virtual machine 

More about Inferno can be found at the [What is Inferno](http://inferno-os.org/inferno/index.html)
page at [inferno-os.org](http://inferno-os.org/inferno/index.html).

## Branches

Inferno is traditionally built by downloading a tarball containing a snapshot of the mercurial
repository from 2010-01-20 which also contains fonts used by the running systems that aren't
included in the mercurial repository itself as they are licensed differently. A mercurial update
is then performed to update to the latest code.

There is a port of inferno that runs on Android devices called [Hellaphone](https://bitbucket.org/floren/inferno/wiki/Home).
This is built upon a snapshot of the mercurial repository from 2011 with the Android patches on top.

To enable easy diffing of the different versions and cherry-picking of commits I've put all these two
snapshots of Inferno in this import as well. The branches are:

<dl>
  <dt>20100120</dt>
  <dd>Snapshot with the additional fonts</dd>
  <dt>hellaphone<dt>
  <dd>Hellaphone Android port</dd>
  <dt>inferno</dt>
  <dd>Direct import of the Inferno mercurial repository</dd>
  <dt>master</dt>
  <dd>Everything needed for building inferno on Linux. It is a recent working version of the `inferno`
      branch with the additional fonts from the `20100120` branch and any additional work in progress patches
      to work around existing issues.
  </dd>
</dl>

## Building

To build on Linux:

    $ git clone https://github.com/doublec/inferno
    $ cd inferno
    $ sh Mkdirs
    $ .. edit `mkconfig` to so the following entries are set to these values...
    ROOT=/root/of/the/inferno/git/clone
    SYSHOST=Linux
    OBJTYPE=386
    $ export PATH=$PATH:`pwd`/Linux/386/bin
    $ ./makemk.sh
    $ mk nuke
    $ mk install

## Running

Run a command line shell with `emu`:

    $ emu
    ; ls
    ...file listing...

You can run the GUI with:

    $ emu -g1024x768
    ; wm/wm
    ...window system appears...

The `-g` command line options sets the size of the Inferno OS host window.

Inferno includes a JIT for the virtual machine that can be enabled with the `-c1` switch:

    $ emu -g1024x768 -c1

## Users

For production use you'll need to set up usernames and some default files in the user home directory. This is done by copying the default `usr/inferno` directory as follows:

    $ cp -r usr/inferno usr/$USER
    $ emu -g1024x767 -c1
    $ emu 
    ; wm/wm wm/logon -u username

A shell script containing the following can be run from anywhere to start inferno:

    export PATH=$PATH:/path/to/inferno/root/Linux/386/bin
    export EMU="-r/path/to/inferno/root -c1 -g1024x768"
    exec emu $* /dis/sh.dis -a -c "wm/wm wm/logon -u $USER"

## Other information

Some places to get further information on Inferno

* [inferno-os.org](http://inferno-os.org/)
* #inferno on irc.freenode.net
* [inferno-os google group](https://groups.google.com/forum/#!forum/inferno-os)
* Pete Elmore's [debu.gs blog](http://debu.gs/tags/inferno)
* My [articles about Inferno](http://bluishcoder.co.nz/tags/inferno/)

This repository is set up mainly to make it easier for me to tinker with Inferno on Android and Linux desktop and I'm sharing it in case others find it useful. For the official work on Inferno and the official source see [inferno-os.org](http://inferno-os.org).
