#!/sbin/busybox sh
#
# Relax.sh - by Javilonas
#

busy=/sbin/busybox;

# lmk tweaks for fewer empty background processes
minfree=12288,15360,18432,21504,24576,30720;
lmk=/sys/module/lowmemorykiller/parameters/minfree;
minboot=`cat $lmk`;
while sleep 1; do
  if [ `cat $lmk` != $minboot ]; then
    [ `cat $lmk` != $minfree ] && echo $minfree > $lmk || exit;
  fi;
done &

# wait for systemui and increase its priority
while sleep 1; do
  if [ `$busy pidof com.android.systemui` ]; then
    systemui=`$busy pidof com.android.systemui`;
    $busy renice -18 $systemui;
    $busy echo -17 > /proc/$systemui/oom_adj;
    $busy chmod 100 /proc/$systemui/oom_adj;
    exit;
  fi;
done &

# lmk whitelist for common launchers and increase launcher priority
list="com.android.launcher com.sec.android.app.launcher com.google.android.googlequicksearchbox org.adw.launcher org.adwfreak.launcher net.alamoapps.launcher com.anddoes.launcher com.android.lmt com.chrislacy.actionlauncher.pro com.cyanogenmod.trebuchet com.gau.go.launcherex com.gtp.nextlauncher com.miui.mihome2 com.mobint.hololauncher com.mobint.hololauncher.hd com.qihoo360.launcher com.teslacoilsw.launcher com.teslacoilsw.launcher.prime com.tsf.shell org.zeam";
while sleep 60; do
  for class in $list; do
    if [ `$busy pgrep $class | head -n 1` ]; then
      launcher=`$busy pgrep $class`;
      $busy echo -17 > /proc/$launcher/oom_adj;
      $busy chmod 100 /proc/$launcher/oom_adj;
      $busy renice -18 $launcher;
    fi;
  done;
  exit;
done &

