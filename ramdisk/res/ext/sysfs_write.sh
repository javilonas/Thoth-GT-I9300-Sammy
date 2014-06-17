#!/sbin/busybox sh
#

#FIX sysfs_write error logcat (By Javilonas)
if [ ! -f /sys/class/input/input1/enabled ]; 
then
  /sbin/busybox printf "0" > /sys/class/input/input1/enabled
  /sbin/busybox chown 0.0 /sys/class/input/input1/enabled
  /sbin/busybox chmod 0666 /sys/class/input/input1/enabled
  /sbin/busybox chmod -R 0666 /sys/class/input/input1/enabled
fi

if [ ! -f /sys/class/input/input8/enabled ]; 
then
  /sbin/busybox printf "0" > /sys/class/input/input8/enabled
  /sbin/busybox chown 0.0 /sys/class/input/input8/enabled
  /sbin/busybox chmod 0666 /sys/class/input/input8/enabled
  /sbin/busybox chmod -R 0666 /sys/class/input/input8/enabled
fi
