#!/sbin/busybox sh
#
# Tweaks - by Javilonas
#

# CACHE AUTO CLEAN

echo "0" > /proc/sys/vm/swappiness
echo "3" > /proc/sys/vm/drop_caches

# disable cpuidle log
echo "0" > /sys/module/cpuidle_exynos4/parameters/log_en

# Miscellaneous tweaks
echo "524488" > /proc/sys/fs/file-max
echo "33200" > /proc/sys/fs/inotify/max_queued_events
echo "584" > /proc/sys/fs/inotify/max_user_instances
echo "10696" > /proc/sys/fs/inotify/max_user_watches
echo "0" > /proc/sys/vm/block_dump
echo "5" > /proc/sys/vm/laptop_mode
echo "0" > /proc/sys/vm/panic_on_oom 
echo "8" > /proc/sys/vm/page-cluster
echo "10" > /proc/sys/fs/lease-break-time
echo "65836" > /proc/sys/kernel/msgmni
echo "65836" > /proc/sys/kernel/msgmax
echo "512 512000 256 2048" > /proc/sys/kernel/sem
echo "268535656" > /proc/sys/kernel/shmmax
echo "525488" > /proc/sys/kernel/threads-max
echo "1" > /proc/sys/vm/oom_kill_allocating_task

# Tweaks internos
echo "2" > /sys/devices/system/cpu/sched_mc_power_savings
echo "0" > /proc/sys/kernel/randomize_va_space
echo "3" > /sys/module/cpuidle_exynos4/parameters/enable_mask

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
echo "10" > /proc/sys/net/ipv4/tcp_fin_timeout
echo "2" > /proc/sys/net/ipv4/tcp_ecn
echo "524288" > /proc/sys/net/core/wmem_max
echo "524288" > /proc/sys/net/core/rmem_max
echo "262144" > /proc/sys/net/core/rmem_default
echo "262144" > /proc/sys/net/core/wmem_default
echo "20480" > /proc/sys/net/core/optmem_max
echo "4096 87380 8388608" > /proc/sys/net/ipv4/tcp_wmem
echo "4096 87380 8388608" > /proc/sys/net/ipv4/tcp_rmem
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

# Incremento de memoria ram
/sbin/busybox setprop dalvik.vm.heapsize 192m
