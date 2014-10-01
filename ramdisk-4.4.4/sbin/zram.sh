#!/sbin/busybox sh
#
# Zram 128MB x 4 = 512MB
#

# Tamaño Zram
zram_size=512
cpuinfo_min_freq=600000

echo 0 > /proc/sys/vm/swappiness

chmod 777 /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq
echo $cpuinfo_min_freq > /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq
chmod 0644 /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq

chmod 777 /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq
echo $scaling_min_freq > /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq
chmod 0644 /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq

chmod 777 /sys/power/cpufreq_min_limit
echo $cpufreq_min_limit > /sys/power/cpufreq_min_limit
chmod 0644 /sys/power/cpufreq_min_limit

# ZRAM
if [ $zram_size -gt 0 ]; then
echo `expr $zram_size \* 1024 \* 256` > /sys/devices/virtual/block/zram0/disksize
/sbin/busybox swapoff /dev/block/zram0 > /dev/null 2>&1
echo 1 > /sys/devices/virtual/block/zram0/reset
echo 1 > /sys/devices/virtual/block/zram0/initstate
/sbin/busybox mkswap /dev/block/zram0 > /dev/null 2>&1
/sbin/busybox swapon -p 2 /dev/block/zram0 > /dev/null 2>&1

echo `expr $zram_size \* 1024 \* 256` > /sys/devices/virtual/block/zram1/disksize
/sbin/busybox swapoff /dev/block/zram1 > /dev/null 2>&1
echo 1 > /sys/devices/virtual/block/zram1/reset
echo 1 > /sys/devices/virtual/block/zram1/initstate
/sbin/busybox mkswap /dev/block/zram1 > /dev/null 2>&1
/sbin/busybox swapon -p 2 /dev/block/zram1 > /dev/null 2>&1

echo `expr $zram_size \* 1024 \* 256` > /sys/devices/virtual/block/zram2/disksize
/sbin/busybox swapoff /dev/block/zram2 > /dev/null 2>&1
echo 1 > /sys/devices/virtual/block/zram2/reset
echo 1 > /sys/devices/virtual/block/zram2/initstate
/sbin/busybox mkswap /dev/block/zram2 > /dev/null 2>&1
/sbin/busybox swapon -p 2 /dev/block/zram2 > /dev/null 2>&1

echo `expr $zram_size \* 1024 \* 256` > /sys/devices/virtual/block/zram3/disksize
/sbin/busybox swapoff /dev/block/zram3 > /dev/null 2>&1
echo 1 > /sys/devices/virtual/block/zram3/reset
echo 1 > /sys/devices/virtual/block/zram3/initstate
/sbin/busybox mkswap /dev/block/zram3 > /dev/null 2>&1
/sbin/busybox swapon -p 2 /dev/block/zram3 > /dev/null 2>&1
fi

