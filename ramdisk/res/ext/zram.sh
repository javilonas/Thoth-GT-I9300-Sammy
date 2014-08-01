#!/sbin/busybox sh
#
# Zram 300MB
#

#Zram0
/sbin/busybox swapoff /dev/block/zram0
echo 1 > /sys/block/zram0/reset
echo 314572800 > /sys/block/zram0/disksize
echo 1 > /sys/block/zram0/initstate
/sbin/busybox mkswap /dev/block/zram0
/sbin/busybox swapon -p 2 /dev/block/zram0
