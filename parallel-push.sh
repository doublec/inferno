#!/bin/sh

for i in `ls`
do
	adb push $i /data/inferno/$i &
done

