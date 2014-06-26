#!/sbin/busybox sh
#
# Tweaks - by Javilonas
#

# CACHE AUTO CLEAN

echo "3" > /proc/sys/vm/drop_caches

# disable cpuidle log
echo "0" > /sys/module/cpuidle_exynos4/parameters/log_en

# Miscellaneous tweaks
echo "0" > /proc/sys/vm/block_dump
echo "5" > /proc/sys/vm/laptop_mode
echo "0" > /proc/sys/vm/panic_on_oom 
echo "8" > /proc/sys/vm/page-cluster

# Multicore Power Saving (0 = Disable, 1 = Enable, 2 = Enable + more aggressive then 1)
echo "0" > /sys/devices/system/cpu/sched_mc_power_savings

echo "0" > /proc/sys/kernel/randomize_va_space

# enable idle+LPA
echo "2" > /sys/module/cpuidle_exynos4/parameters/enable_mask

# IPv6 privacy tweak
echo "2" > /proc/sys/net/ipv6/conf/all/use_tempaddr

# TCP tweaks
echo "1" > /proc/sys/net/ipv4/tcp_low_latency
echo "1" > /proc/sys/net/ipv4/tcp_timestamps
echo "1" > /proc/sys/net/ipv4/tcp_tw_reuse
echo "1" > /proc/sys/net/ipv4/tcp_sack
echo "1" > /proc/sys/net/ipv4/tcp_dsack
echo "1" > /proc/sys/net/ipv4/tcp_tw_recycle
echo "1" > /proc/sys/net/ipv4/tcp_window_scaling
echo "1" > /proc/sys/net/ipv4/tcp_moderate_rcvbuf
echo "1" > /proc/sys/net/ipv4/route/flush
echo "2" > /proc/sys/net/ipv4/tcp_syn_retries
echo "2" > /proc/sys/net/ipv4/tcp_synack_retries
echo "5" > /proc/sys/net/ipv4/tcp_keepalive_probes
echo "10" > /proc/sys/net/ipv4/tcp_keepalive_intvl
echo "10" > /proc/sys/net/ipv4/tcp_fin_timeout
echo "2" > /proc/sys/net/ipv4/tcp_ecn
echo "424288" > /proc/sys/net/core/wmem_max
echo "424288" > /proc/sys/net/core/rmem_max
echo "262144" > /proc/sys/net/core/rmem_default
echo "262144" > /proc/sys/net/core/wmem_default
echo "20480" > /proc/sys/net/core/optmem_max
echo "4096 87380 524388" > /proc/sys/net/ipv4/tcp_wmem
echo "4096 87380 524388" > /proc/sys/net/ipv4/tcp_rmem
echo "4096" > /proc/sys/net/ipv4/udp_rmem_min
echo "4096" > /proc/sys/net/ipv4/udp_wmem_min

# reduce txqueuelen to 0 to switch from a packet queue to a byte one
NET=`ls -d /sys/class/net/*`
for i in $NET 
do
echo "0" > $i/tx_queue_len

done

# Turn off debugging for certain modules
echo "0" > /sys/module/wakelock/parameters/debug_mask
echo "0" > /sys/module/userwakelock/parameters/debug_mask
echo "0" > /sys/module/earlysuspend/parameters/debug_mask
echo "0" > /sys/module/alarm/parameters/debug_mask
echo "0" > /sys/module/alarm_dev/parameters/debug_mask
echo "0" > /sys/module/binder/parameters/debug_mask
echo "0" > /sys/module/lowmemorykiller/parameters/debug_level
echo "0" > /sys/module/mali/parameters/mali_debug_level
echo "0" > /sys/module/ump/parameters/ump_debug_level
echo "0" > /sys/module/kernel/parameters/initcall_debug
echo "0" > /sys/module/xt_qtaguid/parameters/debug_mask

# Otros Misc tweaks
/sbin/busybox mount -t debugfs none /sys/kernel/debug
echo NO_NORMALIZED_SLEEPER > /sys/kernel/debug/sched_features
echo NO_GENTLE_FAIR_SLEEPERS > /sys/kernel/debug/sched_features
echo NO_START_DEBIT > /sys/kernel/debug/sched_features
echo NO_WAKEUP_PREEMPT > /sys/kernel/debug/sched_features
echo NEXT_BUDDY > /sys/kernel/debug/sched_features
echo ARCH_POWER > /sys/kernel/debug/sched_features
echo SYNC_WAKEUPS > /sys/kernel/debug/sched_features
echo HRTICK > /sys/kernel/debug/sched_features

# Fix para problemas Con aplicaciones
/sbin/busybox setprop ro.kernel.android.checkjni 0

# Incremento de memoria ram
/sbin/busybox setprop dalvik.vm.heapstartsize 6m
/sbin/busybox setprop dalvik.vm.heapgrowthlimit 48m
/sbin/busybox setprop dalvik.vm.heapsize 192m

/sbin/busybox setprop ro.HOME_APP_ADJ -17

# Desactivar fast Dormancy
/sbin/busybox setprop ro.semc.enable.fast_dormancy false

# Tiempo de escaneado wifi (ahorra + batería)
/sbin/busybox setprop wifi.supplicant_scan_interval 480
