#!/sbin/busybox sh
#
# Script inicio TPowerCC
#

SYSTEM_DEVICE="/dev/block/mmcblk0p9"

# Inicio lonas-init.sh
/sbin/busybox mount -o remount,rw -t ext4 $SYSTEM_DEVICE /system
/sbin/busybox mount -t rootfs -o remount,rw rootfs

/sbin/busybox rm /system/bin/reboot
/sbin/busybox ln -s /sbin/toolbox /system/bin/reboot

if [ ! -f /system/xbin/busybox ]; then
/sbin/busybox ln -s /sbin/busybox /system/xbin/busybox
/sbin/busybox ln -s /sbin/busybox /system/xbin/pkill
fi

if [ ! -f /system/bin/busybox ]; then
/sbin/busybox ln -s /sbin/busybox /system/bin/busybox
/sbin/busybox ln -s /sbin/busybox /system/bin/pkill
fi

# Enlace simbólico a modulos
if [ ! -f /system/lib/modules ]; then
/sbin/busybox mkdir -p /system/lib
/sbin/busybox ln -s /lib/modules/ /system/lib
fi

# soporte TPowerCC
/sbin/busybox rm /data/.tpowercc/tpowercc.xml
/sbin/busybox rm /data/.tpowercc/action.cache

/res/init_uci.sh select default
/res/init_uci.sh apply

/sbin/busybox sync

/sbin/busybox mount -t rootfs -o remount,ro rootfs
/sbin/busybox mount -o remount,ro -t ext4 $SYSTEM_DEVICE /system

