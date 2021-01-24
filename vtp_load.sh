#!/bin/sh

#this script inserts the module and creates the char device using mknod.

module="vtp"
device="vtp"
mode="664"	#user, group [rw] others [r]

#insert module
/sbin/insmod ./$module.ko $* || exit 1

#remove devices if already exist
rm -f /dev/${device}[0]

#find the major number
major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)

#and finally, make the char device
mknod /dev/${device}0 c $major 0

#now give appropriate permissions to other users, since we have been
#invoked by superuser
group="sudo" #we'll give write permissions to the sudo group

chgrp $group /dev/${device}0
chmod $mode /dev/${device}0
