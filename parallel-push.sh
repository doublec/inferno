#!/bin/sh

for i in `ls`
do
	adb push $i /data/inferno/$i &
done
adb shell 777 /data/inferno/dis/startzygote
adb shell 777 /data/inferno/dis/shutdown

