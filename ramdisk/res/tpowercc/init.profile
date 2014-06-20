# CONFIG TPowerCC
# Rev 01
CONFIG_BOOTED=true
scaling_governor=pegasusq
internal_iosched=row
sd_iosched=row
internal_read_ahead_kb=512
sd_read_ahead_kb=1024
Dyn_fsync_active=on
CONTROLSWITCH_GPU=on
gpu_control_threshold=62% 54% 82% 77% 82% 77% 82% 77%
gputhreshold_predef=1
gpu_control_freq=160 266 350 440 533
gpufreq_predef=0
gpu_control_uV=875000 900000 950000 1025000 1075000
gpuvolt_predef=0
