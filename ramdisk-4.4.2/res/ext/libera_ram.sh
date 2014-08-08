#!/sbin/busybox sh
#
# libera pagecache cada 3 horas si esta está por debajo de 20360 kbytes
# 

/sbin/busybox renice 19 `pidof libera_ram.sh`
DROP_ONE="1"
DROP_TWO="2"
DROP_THREE="3"
FREE=`free -m | grep -i mem | awk '{print $4}'`  

while [ 1 ];
do
        if [ $FREE -lt 20360 ]; then
                sync
                sleep 3
		echo $DROP_ONE > /proc/sys/vm/drop_caches
		echo $DROP_TWO > /proc/sys/vm/drop_caches
		echo $DROP_THREE > /proc/sys/vm/drop_caches
		sync; echo $DROP_THREE > /proc/sys/vm/drop_caches
        fi
sleep 10800
done
