#!/bin/sh

echo "This script will automatically modify your hardware! Use with caution!"
echo "After running this script, your phone should automatically boot into Inferno, if you have /data/inferno installed"
echo "If you do not wish to take this step, press Ctrl-C in the next 10 seconds"
echo "Waiting 10 seconds..."
sleep 5
echo "Waiting 5 seconds..."
sleep 5
echo "Going for it!"
echo Grabbing the current boot image...
adb shell "cat /dev/mtd/mtd2 > /data/boot.img"
adb pull /data/boot.img work/boot.img

echo Pushing the startup script
adb shell "mount -o remount,rw /system"
adb push ./inferno.sh /system/bin/inferno.sh

echo Pushing the boot chooser
adb push ./bootpicker/picker /data/picker

echo Unpacking, modifying, and repacking the boot image
cd work
../unpack-bootimg.pl boot.img
cp ../init.rc boot.img-ramdisk/init.rc
../repack-bootimg.pl boot.img-kernel.gz boot.img-ramdisk boot-inferno.img

echo Rebooting into the bootloader
adb reboot-bootloader
echo Flashing the ROM
fastboot flash boot boot-inferno.img

echo Cleaning up. Will leave boot-inferno.img just in case.
rm -r boot.img*
