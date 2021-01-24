#!/bin/sh

module="vtp"
device="vtp"

/sbin/rmmod $module
rm -f /dev/${device}[0]
