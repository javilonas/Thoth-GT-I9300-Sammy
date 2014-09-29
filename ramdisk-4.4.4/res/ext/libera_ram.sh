#!/sbin/busybox sh
#
# libera pagecache y swap cada 20 minutos si esta está por debajo de 204800 kbytes (200 MB)
# Script creado por Javilonas
# 

/sbin/busybox renice 19 `pidof libera_ram.sh`
DROP_ONE="1"
DROP_TWO="2"
DROP_THREE="3"
FREE=`free -m | grep -i mem | awk '{print $4}'`  

while [ 1 ];
do
        if [ $FREE -lt 204800 ]; then
                /sbin/busybox sync
                /sbin/busybox sleep 3
		echo $DROP_ONE > /proc/sys/vm/drop_caches
		echo $DROP_TWO > /proc/sys/vm/drop_caches
		echo $DROP_THREE > /proc/sys/vm/drop_caches
		/sbin/busybox sync; echo $DROP_THREE > /proc/sys/vm/drop_caches
                /sbin/busybox sync
		/sbin/busybox swapoff /dev/block/vnswap0 > /dev/null 2>&1
		/sbin/busybox swapoff /dev/block/zram0 > /dev/null 2>&1
		/sbin/busybox swapoff /dev/block/zram1 > /dev/null 2>&1
		/sbin/busybox swapoff /dev/block/zram2 > /dev/null 2>&1
		/sbin/busybox swapoff /dev/block/zram3 > /dev/null 2>&1
		/sbin/busybox swapon -p 2 /dev/block/vnswap0 > /dev/null 2>&1
		/sbin/busybox swapon -p 2 /dev/block/zram0 > /dev/null 2>&1
		/sbin/busybox swapon -p 2 /dev/block/zram1 > /dev/null 2>&1
		/sbin/busybox swapon -p 2 /dev/block/zram2 > /dev/null 2>&1
		/sbin/busybox swapon -p 2 /dev/block/zram3 > /dev/null 2>&1
                /sbin/busybox sync
        fi
sleep 1200
done
