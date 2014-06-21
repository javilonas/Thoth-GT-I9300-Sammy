#!/sbin/busybox sh
#
# Temporary reboot command hack
#

SYSTEM_DEVICE="/dev/block/mmcblk0p9"

/sbin/busybox mount -o remount,rw -t ext4 $SYSTEM_DEVICE /system
/sbin/busybox rm /system/bin/reboot
/sbin/busybox ln -s /sbin/toolbox /system/bin/reboot
/sbin/busybox mount -o remount,ro -t ext4 $SYSTEM_DEVICE /system
