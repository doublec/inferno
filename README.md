# Inferno 4th Edition

This is an unofficial conversion of the [Hellaphone](https://bitbucket.org/floren/inferno/wiki/Home) mercurial repository to
git performed by the [hg-git](http://hg-git.github.io/) mercurial plugin.

It includes patches to build using more recent Android versions and tools. I have only tested on a Nexus S but
am working towards getting it working on a Nexus 5.

## Building

The build step is fairly involved. You will need as a working Inferno installation for Linux. This can be built by
following the build instructions in the [master](https://github.com/doublec/inferno) branch. Make sure the
`Linux/386/bin` directory is in your path.

These instructions perform a build using Mozilla's Boot to Gecko project as a base. It's also possible to build
based on Android AOSP but these steps are not yet documented.

### Build B2G

Use the `nexus-5` B2G configuration. The resulting Inferno binary will run on other devices. We just use this to
get libraries and optionally use as a lightweight OS base when installed.

    $ git clone https://github.com/mozilla-b2g/B2G b2g
    $ cd b2g
    $ ./config.sh nexus-5
    $ ./build.sh
    $ export B2G_ROOT=`pwd`

This will take a long time and many GBs of data will be downloaded. The `B2G_ROOT` environment variable is used in the B2G build script.

### Install the Android Standalone Toolkit

Building Inferno requires generating a standalone toolchain from the Android NDK. This results in a vesion of gcc and libraries that can be run from conventional makefiles and configure scripts.

To generate a standalone toolchain I used:

    $NDK_ROOT/build/tools/make-standalone-toolchain.sh \
          --platform=android-8 \
          --install-dir=$STK_ROOT

Replace `NDK_ROOT` with the path to the Android SDK and `STK_ROOT` with the destination where you want the standalone toolkit installed. The android-8 in the platform makes the standalone toolkit generate applications for FroYo and above.

Adding `$STK_ROOT/bin` to the PATH will make the compiler available:

    export PATH=$PATH:$STK_ROOT/bin

There is an issue with building Inferno regarding stdlib.h. To prevent a compile error around the use of `wchar_t` I modified `$STK_ROOT/sysroot/usr/include/stdlib.h`: at about line 169 to change #if 1 to #ifndef INFERNO to prevent the `wchar_t` usage of mblen and friends from being used.

### Build Android Inferno

The installation directory for the Inferno build on the host system must match that of the target system. The steps below create and use `/data/inferno`:

    $ sudo mkdir /data
    $ sudo chown -R $USER:$USER /data
    $ cd /data
    $ git clone https://github.com/doublec/inferno
    $ cd inferno
    $ git checkout -b hellaphone origin/hellaphone
    $ mk nuke
    $ mk install

## Installing

To install on a rooted Nexus S already running Android 4.x, connect the phone via `adb` and run:

    $ ./parallel-push.sh

This will copy everything to `/data/inferno` on the phone.

These instructions will be updated later to describe a full install with a B2G base.

## Running 

From `adb` you can shut down the Android system and run Inferno with:

    $ adb shell
    # stop zygote
    # stop bootanim
    # cd /data/inferno
    # ./Linux/arm/bin/emu-g
    ; wm/wm

On a B2G system replace the stopping of zygote and bootanim with stopping b2g:

    $ adb shell
    # stop b2g
    # cd /data/inferno
    # ./Linux/arm/bin/emu-g
    ; wm/wm
