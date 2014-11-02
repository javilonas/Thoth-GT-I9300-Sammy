#!/sbin/busybox sh
#
# Tweaks - by Javilonas
#

# CACHE AUTO CLEAN

echo "3" > /proc/sys/vm/drop_caches

# disable cpuidle log
echo "0" > /sys/module/cpuidle_exynos4/parameters/log_en

# Miscellaneous tweaks
echo "8" > /proc/sys/vm/page-cluster

echo "0" > /proc/sys/kernel/randomize_va_space

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
/sbin/busybox setprop dalvik.vm.heapstartsize 5m
/sbin/busybox setprop dalvik.vm.heapgrowthlimit 48m
/sbin/busybox setprop dalvik.vm.heapsize 192m
/sbin/busybox setprop dalvik.vm.heaptargetutilization 0.75
/sbin/busybox setprop dalvik.vm.heapminfree 512k
/sbin/busybox setprop dalvik.vm.heapmaxfree 2m

/sbin/busybox setprop ro.HOME_APP_ADJ -17

# Desactivar fast Dormancy
/sbin/busybox setprop ro.semc.enable.fast_dormancy false

# Tiempo de escaneado wifi (ahorra + batería)
/sbin/busybox setprop wifi.supplicant_scan_interval 480

