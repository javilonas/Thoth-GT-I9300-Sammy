#!/sbin/busybox sh
#

#SideSync
if [ ! -f /dev/android_ssusbcon ]; 
then
  /sbin/busybox printf "CONN" > /dev/android_ssusbcon
  /sbin/busybox chown 0.0 /dev/android_ssusbcon
  /sbin/busybox chmod 0666 /dev/android_ssusbcon
  /sbin/busybox chmod -R 0666 /dev/android_ssusbcon 
fi
