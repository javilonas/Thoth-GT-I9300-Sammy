scaling_governor=pegasusq
internal_iosched=row
sd_iosched=row
internal_read_ahead_kb=256
sd_read_ahead_kb=512
min_free_kbytes=8192
dirty_writeback_centisecs=1500
dirty_expire_centisecs=300
swappiness=30
Dyn_fsync_active=on
ignore_unstable_power=off
ignore_safety_margin=off
CONTROLSWITCH_GPU=on
gpu_control_threshold=50% 42% 70% 65% 70% 65% 70% 65%
gputhreshold_predef=2
gpu_control_freq=266 350 440 533 640
gpufreq_predef=2
gpu_control_uV=875000 900000 950000 1025000 1075000
gpuvolt_predef=0
minfree_lkm=12288,15360,21504,32768,49152,65536
minfree_predef=2
charge_level_usb=900
chargeusbcurrent_predef=2
charge_level_ac=1200
chargeaccurrent_predef=1
CONTROLSWITCH_CPU=on
CONTROLSWITCH_LMK=on
CONFIG_BOOTED=true
