/*
 *  drivers/cpufreq/cpufreq_impulse.c
 *
 *  Copyright (C)  2001 Russell King
 *            (C)  2003 Venkatesh Pallipadi <venkatesh.pallipadi@intel.com>.
 *                      Jun Nakajima <jun.nakajima@intel.com>
 *            (C)  2009 Alexander Clouter <alex@digriz.org.uk>
 *            (C)  2013 Zane Zaminsky <cyxman@yahoo.com>
 *            (C)  2014 Javier Sayago <admin@lonasdigital.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * 
 */

//  Added a sysfs interface to display current impulse version
#define IMPULSE_VERSION "0.2"

//  Allow to include or exclude legacy mode (support for SGS3/Note II only and max scaling freq 1800mhz!)
//#define ENABLE_LEGACY_MODE

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cpufreq.h>
#include <linux/cpu.h>
#include <linux/jiffies.h>
#include <linux/kernel_stat.h>
#include <linux/mutex.h>
#include <linux/hrtimer.h>
#include <linux/tick.h>
#include <linux/ktime.h>
#include <linux/sched.h>
#include <linux/earlysuspend.h>

// cpu load trigger
#define DEF_SMOOTH_UP (75)

/*
 * dbs is used in this file as a shortform for demandbased switching
 * It helps to keep variable names smaller, simpler
 */

//  midnight and impulse default values
#define DEF_FREQUENCY_UP_THRESHOLD		  (70)
#define DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG	  (68)	//  default for hotplug up threshold for all cpus (cpu0 stays allways on)
#define DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG_FREQ   (0)	//  default for hotplug up threshold frequency for all cpus (0 = disabled)
#define DEF_FREQUENCY_DOWN_THRESHOLD		  (52)
#define DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG	  (55)	//  default for hotplug down threshold for all cpus (cpu0 stays allways on)
#define DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG_FREQ (0)	//  default for hotplug down threshold frequency for all cpus (0 = disabled)
#define DEF_IGNORE_NICE				  (0)	//  default for ignore nice load
#define DEF_FREQ_STEP				  (5)	//  default for freq step at awake
#define DEF_FREQ_STEP_SLEEP			  (5)	//  default for freq step at early suspend

//  LCDFreq Scaling default values
#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
#define LCD_FREQ_KICK_IN_DOWN_DELAY		(20)	//  default for kick in down delay
#define LCD_FREQ_KICK_IN_UP_DELAY		(50)	//  default for kick in up delay
#define LCD_FREQ_KICK_IN_FREQ			(500000)//  default kick in frequency
#define LCD_FREQ_KICK_IN_CORES			(0)	//  number of cores which should be online before kicking in
extern int _lcdfreq_lock(int lock);			//  external lcdfreq lock function
#endif

/*
 * The polling frequency of this governor depends on the capability of
 * the processor. Default polling frequency is 1000 times the transition
 * latency of the processor. The governor will work on any processor with
 * transition latency <= 10mS, using appropriate sampling
 * rate.
 * For CPUs with transition latency > 10mS (mostly drivers with CPUFREQ_ETERNAL)
 * this governor will not work.
 * All times here are in uS.
 */

#define MIN_SAMPLING_RATE_RATIO			(2)

//  Sampling down momentum variables
static unsigned int min_sampling_rate;			//  minimal possible sampling rate
static unsigned int orig_sampling_down_factor;		//  for saving previously set sampling down factor
static unsigned int orig_sampling_down_max_mom;		//  for saving previously set smapling down max momentum

//  search limit for frequencies in scaling table, variables for scaling modes and state flags for deadlock fix/suspend detection
static unsigned int max_scaling_freq_soft = 0;		//  init value for "soft" scaling to 0 = full range
static unsigned int max_scaling_freq_hard = 0;		//  init value for "hard" scaling to 0 = full range
static unsigned int limit_table_end = CPUFREQ_TABLE_END;//  end limit for frequency table in descending table
static unsigned int limit_table_start = 0;		//  start limit for frequency table in decending table
static unsigned int suspend_flag = 0;			//  init value for suspend status. 1 = in early suspend
static unsigned int skip_hotplug_flag = 1;		//  initial start without hotplugging to fix lockup issues
static int scaling_mode_up;				//  fast scaling up mode holding up value during runtime
static int scaling_mode_down;				//  fast scaling down mode holding down value during runtime

//  added hotplug idle threshold and block cycles
#define DEF_HOTPLUG_BLOCK_CYCLES		(0)
#define DEF_HOTPLUG_IDLE_THRESHOLD		(0)
static unsigned int hotplug_idle_flag = 0;
static unsigned int hotplug_down_block_cycles = 0;
static unsigned int hotplug_up_block_cycles = 0;

//  current load & freq. for hotplugging work
static int cur_load = 0;
static int cur_freq = 0;

//  hotplug threshold array
static int hotplug_thresholds[2][8]={
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
    };

//  hotplug threshold frequencies array
static int hotplug_thresholds_freq[2][8]={
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
    };

/*
*  hotplug tuneable init flags for making an exception in tuneable value rules
* which will give apply-order-independence directly after start of the governor
* and will switch back to full check after first apply of values in that tuneables
* keep in mind with this workaround odd values are possible when you are using
* init.d scripts or using saved profiles of any tuning app
*/
static int hotplug_thresholds_tuneable[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

//  support for 2,4 or 8 cores (this will enable/disable hotplug threshold tuneables)
#define MAX_CORES		(4)

// raise sampling rate to SR*multiplier and adjust sampling rate/thresholds/hotplug/scaling/freq limit/freq step on blank screen

//  LCDFreq scaling
#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
static int lcdfreq_lock_current = 0;			//  LCDFreq scaling lock switch
static int prev_lcdfreq_lock_current;			//  for saving previously set lock state
static int prev_lcdfreq_enable;				//  for saving previously set enabled state
#endif
static unsigned int sampling_rate_awake;
static unsigned int up_threshold_awake;
static unsigned int down_threshold_awake;
static unsigned int smooth_up_awake;
static unsigned int freq_limit_awake;			//  for saving freqency limit awake value
static unsigned int fast_scaling_awake;			//  for saving fast scaling awake value
static unsigned int freq_step_awake;			//  for saving frequency step awake value
static unsigned int disable_hotplug_awake;		//  for daving hotplug switch
static unsigned int hotplug1_awake;			//  for saving hotplug1 threshold awake value
#if (MAX_CORES == 4 || MAX_CORES == 8)
static unsigned int hotplug2_awake;			//  for saving hotplug2 threshold awake value
static unsigned int hotplug3_awake;			//  for saving hotplug3 threshold awake value
#endif
#if (MAX_CORES == 8)
static unsigned int hotplug4_awake;			//  for saving hotplug4 threshold awake value
static unsigned int hotplug5_awake;			//  for saving hotplug5 threshold awake value
static unsigned int hotplug6_awake;			//  for saving hotplug6 threshold awake value
static unsigned int hotplug7_awake;			//  for saving hotplug7 threshold awake value
#endif
static unsigned int sampling_rate_asleep;		//  for setting sampling rate value on early suspend
static unsigned int up_threshold_asleep;		//  for setting up threshold value on early suspend
static unsigned int down_threshold_asleep;		//  for setting down threshold value on early suspend
static unsigned int smooth_up_asleep;			//  for setting smooth scaling value on early suspend
static unsigned int freq_limit_asleep;			//  for setting frequency limit value on early suspend
static unsigned int fast_scaling_asleep;		//  for setting fast scaling value on early suspend
static unsigned int freq_step_asleep;			//  for setting freq step value on early suspend
static unsigned int disable_hotplug_asleep;		//  for setting hotplug on/off on early suspend

//  midnight and impulse momentum defaults
#define LATENCY_MULTIPLIER			(1000)
#define MIN_LATENCY_MULTIPLIER			(100)
#define DEF_SAMPLING_DOWN_FACTOR		(1)	//  default for sampling down factor (stratosk default = 4) here disabled by default
#define MAX_SAMPLING_DOWN_FACTOR		(100000)//  changed from 10 to 100000 for sampling down momentum implementation
#define TRANSITION_LATENCY_LIMIT		(10 * 1000 * 1000)

//  Sampling down momentum
#define DEF_SAMPLING_DOWN_MOMENTUM		(0)	//  sampling down momentum disabled by default
#define DEF_SAMPLING_DOWN_MAX_MOMENTUM		(0)	//  default for tuneable sampling_down_max_momentum stratosk default=16, here disabled by default
#define DEF_SAMPLING_DOWN_MOMENTUM_SENSITIVITY  (50)	//  default for tuneable sampling_down_momentum_sensitivity
#define MAX_SAMPLING_DOWN_MOMENTUM_SENSITIVITY  (1000)	//  max value for tuneable sampling_down_momentum_sensitivity

//  midnight and impulse defaults for suspend
#define DEF_SAMPLING_RATE_SLEEP_MULTIPLIER	(2)	//  default for tuneable sampling_rate_sleep_multiplier
#define MAX_SAMPLING_RATE_SLEEP_MULTIPLIER	(4)	//  maximum for tuneable sampling_rate_sleep_multiplier
#define DEF_UP_THRESHOLD_SLEEP			(90)	//  default for tuneable up_threshold_sleep
#define DEF_DOWN_THRESHOLD_SLEEP		(44)	//  default for tuneable down_threshold_sleep
#define DEF_SMOOTH_UP_SLEEP			(100)	//  default for tuneable smooth_up_sleep

/*
*  Hotplug Sleep: 0 do not touch hotplug settings on early suspend, so that all cores will be online
* the value is equivalent to the amount of cores which should be online on early suspend
*/

#define DEF_HOTPLUG_SLEEP			(0)	//  default for tuneable hotplug_sleep
#define DEF_GRAD_UP_THRESHOLD			(25)	//  default for grad up threshold

/*
*  Frequency Limit: 0 do not limit frequency and use the full range up to policy->max limit
* values policy->min to policy->max in khz
*/

#define DEF_FREQ_LIMIT				(0)	//  default for tuneable freq_limit
#define DEF_FREQ_LIMIT_SLEEP			(0)	//  default for tuneable freq_limit_sleep

/*
*  Fast Scaling: 0 do not activate fast scaling function
* values 1-4 to enable fast scaling with normal down scaling 5-8 to enable fast scaling with fast up and down scaling
*/

#define DEF_FAST_SCALING			(0)	//  default for tuneable fast_scaling
#define DEF_FAST_SCALING_SLEEP			(0)	//  default for tuneable fast_scaling_sleep

struct work_struct hotplug_offline_work;
struct work_struct hotplug_online_work;

static void do_dbs_timer(struct work_struct *work);

struct cpu_dbs_info_s {
	u64 time_in_idle;				//  added exit time handling
	u64 idle_exit_time;				//  added exit time handling
	cputime64_t prev_cpu_idle;
	cputime64_t prev_cpu_wall;
	cputime64_t prev_cpu_nice;
	struct cpufreq_policy *cur_policy;
	struct delayed_work work;
	unsigned int down_skip;				//  Smapling down reactivated
	unsigned int check_cpu_skip;			//  check_cpu skip counter (to avoid potential deadlock because of double locks from hotplugging)
	unsigned int requested_freq;
	unsigned int rate_mult;				//  Sampling down momentum sampling rate multiplier
	unsigned int momentum_adder;			//  Sampling down momentum adder
	int cpu;
	unsigned int enable:1;
	unsigned int prev_load;				//  Early demand var for previous load
	/*
	 * percpu mutex that serializes governor limit change with
	 * do_dbs_timer invocation. We do not want do_dbs_timer to run
	 * when user is changing the governor or limits.
	 */
	struct mutex timer_mutex;
};
static DEFINE_PER_CPU(struct cpu_dbs_info_s, cs_cpu_dbs_info);

static unsigned int dbs_enable;	/* number of CPUs using this policy */

/*
 * dbs_mutex protects dbs_enable in governor start/stop.
 */
static DEFINE_MUTEX(dbs_mutex);

static struct dbs_tuners {
	unsigned int sampling_rate;
	unsigned int sampling_rate_sleep_multiplier;	//  added tuneable sampling_rate_sleep_multiplier
	unsigned int sampling_down_factor;		//  Sampling down factor (reactivated)
	unsigned int sampling_down_momentum;		//  Sampling down momentum tuneable
	unsigned int sampling_down_max_mom;		//  Sampling down momentum max tuneable
	unsigned int sampling_down_mom_sens;		//  Sampling down momentum sensitivity
	unsigned int up_threshold;
	unsigned int up_threshold_hotplug1;		//  added tuneable up_threshold_hotplug1 for core1
	unsigned int up_threshold_hotplug_freq1;	//  added tuneable up_threshold_hotplug_freq1 for core1
#if (MAX_CORES == 4 || MAX_CORES == 8)
	unsigned int up_threshold_hotplug2;		//  added tuneable up_threshold_hotplug2 for core2
	unsigned int up_threshold_hotplug_freq2;	//  added tuneable up_threshold_hotplug_freq1 for core2
	unsigned int up_threshold_hotplug3;		//  added tuneable up_threshold_hotplug3 for core3
	unsigned int up_threshold_hotplug_freq3;	//  added tuneable up_threshold_hotplug_freq1 for core3
#endif
#if (MAX_CORES == 8)
	unsigned int up_threshold_hotplug4;		//  added tuneable up_threshold_hotplug4 for core4
	unsigned int up_threshold_hotplug_freq4;	//  added tuneable up_threshold_hotplug_freq1 for core4
	unsigned int up_threshold_hotplug5;		//  added tuneable up_threshold_hotplug5 for core5
	unsigned int up_threshold_hotplug_freq5;	//  added tuneable up_threshold_hotplug_freq1 for core5
	unsigned int up_threshold_hotplug6;		//  added tuneable up_threshold_hotplug6 for core6
	unsigned int up_threshold_hotplug_freq6;	//  added tuneable up_threshold_hotplug_freq1 for core6
	unsigned int up_threshold_hotplug7;		//  added tuneable up_threshold_hotplug7 for core7
	unsigned int up_threshold_hotplug_freq7;	//  added tuneable up_threshold_hotplug_freq1 for core7
#endif
	unsigned int up_threshold_sleep;		//  added tuneable up_threshold_sleep for early suspend
	unsigned int down_threshold;
	unsigned int down_threshold_hotplug1;		//  added tuneable down_threshold_hotplug1 for core1
	unsigned int down_threshold_hotplug_freq1;	//  added tuneable down_threshold_hotplug_freq1 for core1
#if (MAX_CORES == 4 || MAX_CORES == 8)
	unsigned int down_threshold_hotplug2;		//  added tuneable down_threshold_hotplug2 for core2
	unsigned int down_threshold_hotplug_freq2;	//  added tuneable down_threshold_hotplug_freq1 for core2
	unsigned int down_threshold_hotplug3;		//  added tuneable down_threshold_hotplug3 for core3
	unsigned int down_threshold_hotplug_freq3;	//  added tuneable down_threshold_hotplug_freq1 for core3
#endif
#if (MAX_CORES == 8)
	unsigned int down_threshold_hotplug4;		//  added tuneable down_threshold_hotplug4 for core4
	unsigned int down_threshold_hotplug_freq4;	//  added tuneable down_threshold_hotplug_freq1 for core4
	unsigned int down_threshold_hotplug5;		//  added tuneable down_threshold_hotplug5 for core5
	unsigned int down_threshold_hotplug_freq5;	//  added tuneable down_threshold_hotplug_freq1 for core5
	unsigned int down_threshold_hotplug6;		//  added tuneable down_threshold_hotplug6 for core6
	unsigned int down_threshold_hotplug_freq6;	//  added tuneable down_threshold_hotplug_freq1 for core6
	unsigned int down_threshold_hotplug7;		//  added tuneable down_threshold_hotplug7 for core7
	unsigned int down_threshold_hotplug_freq7;	//  added tuneable down_threshold_hotplug_freq1 for core7
#endif
	unsigned int down_threshold_sleep;		//  added tuneable down_threshold_sleep for early suspend
	unsigned int ignore_nice;
	unsigned int freq_step;
	unsigned int freq_step_sleep;			//  added tuneable freq_step_sleep for early suspend
	unsigned int smooth_up;
	unsigned int smooth_up_sleep;			//  added tuneable smooth_up_sleep for early suspend
	unsigned int hotplug_sleep;			//  added tuneable hotplug_sleep for early suspend
	unsigned int freq_limit;			//  added tuneable freq_limit
	unsigned int freq_limit_sleep;			//  added tuneable freq_limit_sleep
	unsigned int fast_scaling;			//  added tuneable fast_scaling
	unsigned int fast_scaling_sleep;		//  added tuneable fast_scaling_sleep
	unsigned int grad_up_threshold;			//  Early demand grad up threshold tuneable
	unsigned int early_demand;			//  Early demand master switch
	unsigned int disable_hotplug;			//  Hotplug switch
	unsigned int disable_hotplug_sleep;		//  Hotplug switch for sleep
	unsigned int hotplug_block_cycles;		//  Hotplug block cycles
	unsigned int hotplug_idle_threshold;		//  Hotplug idle threshold
#ifdef ENABLE_LEGACY_MODE
	unsigned int legacy_mode;			//  Legacy Mode
#endif
#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
	int lcdfreq_enable;				//  LCDFreq Scaling switch
	unsigned int lcdfreq_kick_in_down_delay;	//  LCDFreq Scaling kick in down delay counter
	unsigned int lcdfreq_kick_in_down_left;		//  LCDFreq Scaling kick in down left counter
	unsigned int lcdfreq_kick_in_up_delay;		//  LCDFreq Scaling kick in up delay counter
	unsigned int lcdfreq_kick_in_up_left;		//  LCDFreq Scaling kick in up left counter
	unsigned int lcdfreq_kick_in_freq;		//  LCDFreq Scaling kick in frequency
	unsigned int lcdfreq_kick_in_cores;		//  LCDFreq Scaling kick in cores
#endif

} dbs_tuners_ins = {
	.up_threshold = DEF_FREQUENCY_UP_THRESHOLD,
	.up_threshold_hotplug1 = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG,			//  set default value for new tuneable
	.up_threshold_hotplug_freq1 = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG_FREQ,		//  set default value for new tuneable
#if (MAX_CORES == 4 || MAX_CORES == 8)
	.up_threshold_hotplug2 = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG,			//  set default value for new tuneable
	.up_threshold_hotplug_freq2 = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG_FREQ,		//  set default value for new tuneable
	.up_threshold_hotplug3 = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG,			//  set default value for new tuneable
	.up_threshold_hotplug_freq3 = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG_FREQ,		//  set default value for new tuneable
#endif
#if (MAX_CORES == 8)
	.up_threshold_hotplug4 = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG,			//  set default value for new tuneable
	.up_threshold_hotplug_freq4 = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG_FREQ,		//  set default value for new tuneable
	.up_threshold_hotplug5 = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG,			//  set default value for new tuneable
	.up_threshold_hotplug_freq5 = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG_FREQ,		//  set default value for new tuneable
	.up_threshold_hotplug6 = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG,			//  set default value for new tuneable
	.up_threshold_hotplug_freq6 = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG_FREQ,		//  set default value for new tuneable
	.up_threshold_hotplug7 = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG,			//  set default value for new tuneable
	.up_threshold_hotplug_freq7 = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG_FREQ,		//  set default value for new tuneable
#endif
	.up_threshold_sleep = DEF_UP_THRESHOLD_SLEEP,					//  set default value for new tuneable
	.down_threshold = DEF_FREQUENCY_DOWN_THRESHOLD,
	.down_threshold_hotplug1 = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG,		//  set default value for new tuneable
	.down_threshold_hotplug_freq1 = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG_FREQ,	//  set default value for new tuneable
#if (MAX_CORES == 4 || MAX_CORES == 8)
	.down_threshold_hotplug2 = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG,		//  set default value for new tuneable
	.down_threshold_hotplug_freq2 = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG_FREQ,	//  set default value for new tuneable
	.down_threshold_hotplug3 = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG,		//  set default value for new tuneable
	.down_threshold_hotplug_freq3 = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG_FREQ,	//  set default value for new tuneable
#endif
#if (MAX_CORES == 8)
	.down_threshold_hotplug4 = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG,		//  set default value for new tuneable
	.down_threshold_hotplug_freq4 = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG_FREQ,	//  set default value for new tuneable
	.down_threshold_hotplug5 = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG,		//  set default value for new tuneable
	.down_threshold_hotplug_freq5 = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG_FREQ,	//  set default value for new tuneable
	.down_threshold_hotplug6 = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG,		//  set default value for new tuneable
	.down_threshold_hotplug_freq6 = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG_FREQ,	//  set default value for new tuneable
	.down_threshold_hotplug7 = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG,		//  set default value for new tuneable
	.down_threshold_hotplug_freq7 = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG_FREQ,	//  set default value for new tuneable
#endif
	.down_threshold_sleep = DEF_DOWN_THRESHOLD_SLEEP,				//  set default value for new tuneable
	.sampling_down_factor = DEF_SAMPLING_DOWN_FACTOR,				//  sampling down reactivated but disabled by default
	.sampling_down_momentum = DEF_SAMPLING_DOWN_MOMENTUM,				//  Sampling down momentum initial disabled
	.sampling_down_max_mom = DEF_SAMPLING_DOWN_MAX_MOMENTUM,			//  Sampling down momentum default for max momentum
	.sampling_down_mom_sens = DEF_SAMPLING_DOWN_MOMENTUM_SENSITIVITY,		//  Sampling down momentum default for sensitivity
	.sampling_rate_sleep_multiplier = DEF_SAMPLING_RATE_SLEEP_MULTIPLIER,		//  set default value for new tuneable
	.ignore_nice = DEF_IGNORE_NICE,							//  set default value for tuneable
	.freq_step = DEF_FREQ_STEP,							//  set default value for new tuneable
	.freq_step_sleep = DEF_FREQ_STEP_SLEEP,						//  set default value for new tuneable
	.smooth_up = DEF_SMOOTH_UP,
	.smooth_up_sleep = DEF_SMOOTH_UP_SLEEP,						//  set default value for new tuneable
	.hotplug_sleep = DEF_HOTPLUG_SLEEP,						//  set default value for new tuneable
	.freq_limit = DEF_FREQ_LIMIT,							//  set default value for new tuneable
	.freq_limit_sleep = DEF_FREQ_LIMIT_SLEEP,					//  set default value for new tuneable
	.fast_scaling = DEF_FAST_SCALING,						//  set default value for new tuneable
	.fast_scaling_sleep = DEF_FAST_SCALING_SLEEP,					//  set default value for new tuneable
	.grad_up_threshold = DEF_GRAD_UP_THRESHOLD,					//  Early demand default for grad up threshold
	.early_demand = 0,								//  Early demand default off
	.disable_hotplug = false,							//  Hotplug switch default off (=hotplugging on)
	.disable_hotplug_sleep = false,							//  Hotplug switch for sleep default off (=hotplugging on)
	.hotplug_block_cycles = DEF_HOTPLUG_BLOCK_CYCLES,				//  Hotplug block cycles default
	.hotplug_idle_threshold = DEF_HOTPLUG_IDLE_THRESHOLD,				//  Hotplug idle threshold default
#ifdef ENABLE_LEGACY_MODE
	.legacy_mode = false,								//  Legacy Mode default off
#endif
#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
	.lcdfreq_enable = false,							//  LCDFreq Scaling default off
	.lcdfreq_kick_in_down_delay = LCD_FREQ_KICK_IN_DOWN_DELAY,			//  LCDFreq Scaling default for down delay
	.lcdfreq_kick_in_down_left = LCD_FREQ_KICK_IN_DOWN_DELAY,			//  LCDFreq Scaling default for down left
	.lcdfreq_kick_in_up_delay = LCD_FREQ_KICK_IN_UP_DELAY,				//  LCDFreq Scaling default for up delay
	.lcdfreq_kick_in_up_left = LCD_FREQ_KICK_IN_UP_DELAY,				//  LCDFreq Scaling default for up left
	.lcdfreq_kick_in_freq = LCD_FREQ_KICK_IN_FREQ,					//  LCDFreq Scaling default for kick in freq
	.lcdfreq_kick_in_cores = LCD_FREQ_KICK_IN_CORES,				//  LCDFreq Scaling default for kick in cores
#endif
	};

unsigned int freq_table_size = 0;							//  upper index limit of freq. table
unsigned int min_scaling_freq = 0;							//  lowest frequency index in global frequency table
int freq_table_order = 1;								//  1 for descending order, -1 for ascending order

/**
 * Smooth scaling conservative governor (by Michael Weingaertner)
 * -----------------------------------------------------------------------
 * -------------- since only in Legacy Mode -----------------
 * This modification makes the governor use two lookup tables holding
 * current, next and previous frequency to directly get a correct
 * target frequency instead of calculating target frequencies with
 * up_threshold and step_up %. The two scaling lookup tables used
 * contain different scaling steps/frequencies to achieve faster upscaling
 * on higher CPU load.
 * -----------------------------------------------------------------------
 * -------------- since only in Legacy Mode -----------------
 * CPU load triggering faster upscaling can be adjusted via SYSFS,
 * VALUE between 1 and 100 (% CPU load):
 * echo VALUE > /sys/devices/system/cpu/cpufreq/impulse/smooth_up
 *
 * improved by Zane Zaminsky and Yank555 2012/13
 */

#define SCALE_FREQ_UP 1
#define SCALE_FREQ_DOWN 2

#ifdef ENABLE_LEGACY_MODE
//  Legacy Mode
#define LEG_FREQ 0
#endif

/*
 * Table modified for use with Samsung I9300 by ZaneZam November 2012
 *			- table modified to reach overclocking frequencies up to 1600mhz
 *			- added fast scaling columns to frequency table
 *			- removed fast scaling colums and use line jumps instead. 4 steps and 2 modes (with/without fast downscaling) possible now
 *			  table modified to reach overclocking frequencies up to 1800mhz
 *			  fixed wrong frequency stepping
 *			  added search limit for more efficent frequency searching and better hard/softlimit handling
 *			- combination of power and normal scaling table to only one array (idea by Yank555)
 *			- scaling logic reworked and optimized by Yank555
 *			- completely removed lookup tables and use the system frequency table instead
 *                        modified scaling logic accordingly (credits to Yank555)
 *			- added check if CPU freq. table is in ascending or descending order and scale accordingly (credits to Yank555)
 *			- reindroduced the "scaling lookup table way" in form of the "Legacy Mode"
 *			- readded forgotten frequency search optimisation
 *			- frequency search optimisation now fully compatible with ascending ordered system frequency tables
 *
 */

#ifdef ENABLE_LEGACY_MODE
/*
 *     Legacy Mode:
 *     Frequency table from version 0.5 extended with fast scaling frequencies
 *     Scaling logic base taken from version 0.4 enhanced with some optimizations
*/

static int leg_freqs[17][7]={
    {1800000,1800000,1700000,1800000,1700000,1800000,1500000},
    {1700000,1800000,1600000,1800000,1600000,1800000,1400000},
    {1600000,1700000,1500000,1800000,1500000,1800000,1300000},
    {1500000,1600000,1400000,1700000,1400000,1600000,1200000},
    {1400000,1500000,1300000,1600000,1300000,1600000,1100000},
    {1300000,1400000,1200000,1500000,1200000,1400000,1000000},
    {1200000,1300000,1100000,1400000,1100000,1400000, 900000},
    {1100000,1200000,1000000,1300000,1000000,1200000, 800000},
    {1000000,1100000, 900000,1200000, 900000,1200000, 700000},
    { 900000,1000000, 800000,1100000, 800000,1000000, 600000},
    { 800000, 900000, 700000,1000000, 700000,1000000, 500000},
    { 700000, 800000, 600000, 900000, 600000, 800000, 400000},
    { 600000, 700000, 400000, 800000, 500000, 800000, 300000},
    { 500000, 600000, 300000, 700000, 400000, 600000, 200000},
    { 400000, 500000, 200000, 600000, 300000, 600000, 200000},
    { 300000, 400000, 200000, 500000, 200000, 400000, 200000},
    { 200000, 300000, 200000, 400000, 200000, 400000, 200000}
};

//  Legacy Mode scaling
static int leg_get_next_freq(unsigned int curfreq, unsigned int updown, unsigned int load) {
    int i=0;

if (load < dbs_tuners_ins.smooth_up)
    {
	for(i = 0; i < 17 ; i++)
	{
	    if(unlikely(curfreq == leg_freqs[i][LEG_FREQ])) {
	    if(dbs_tuners_ins.fast_scaling != 0)
		return leg_freqs[i][updown+4]; // updown 5|6 - fast scaling colums
	    else
		return leg_freqs[i][updown];   // updown 1|2 - normal colums
	    }
	}
    }
    else
    {
	for(i = 0; i < 17; i++)
	{
	    if(unlikely(curfreq == leg_freqs[i][LEG_FREQ])){
	    if(dbs_tuners_ins.fast_scaling != 0)
		return leg_freqs[i][updown+4]; // updown 5|6 - fast scaling colums
	    else
		return leg_freqs[i][updown+2]; // updown 3|4 - power colums
	    }
	}
    }
return (curfreq); // not found
}
#endif

//  Return a valid value between min and max
static int validate_min_max(int val, int min, int max) {

	return min(max(val, min), max);

}

static int mn_get_next_freq(unsigned int curfreq, unsigned int updown, unsigned int load) {

	int i=0;
	int smooth_up_steps=0;			//  smooth_up steps
	struct cpufreq_frequency_table *table;	//  Use system frequency table

	table = cpufreq_frequency_get_table(0);	//  Get system frequency table

	if (load < dbs_tuners_ins.smooth_up)	//  Consider smooth up
		smooth_up_steps=0;		//          load not reached, move by one step
	else
		smooth_up_steps=1;		//          load reached, move by two steps

	for(i = limit_table_start; (table[i].frequency != limit_table_end); i++) { //  added forgotten max scaling search optimization again

		if(unlikely(curfreq == table[i].frequency)) {

			//  We found where we currently are (i)
			if(updown == SCALE_FREQ_UP)

				return	min(	//  Scale up, but don't go above softlimit
						table[max_scaling_freq_soft                                                                                 ].frequency,
						table[validate_min_max((i - 1 - smooth_up_steps - scaling_mode_up  ) * freq_table_order, 0, freq_table_size)].frequency
					);

			else

				return	max(	//  Scale down, but don't go below min. freq.
						table[min_scaling_freq                                                                                      ].frequency,
						table[validate_min_max((i + 1                   + scaling_mode_down) * freq_table_order, 0, freq_table_size)].frequency
					);

			return (curfreq);	//  We should never get here...

		}

	}

	return (curfreq); // not found

}

static inline cputime64_t get_cpu_idle_time_jiffy(unsigned int cpu,
							cputime64_t *wall)
{
	cputime64_t idle_time;
	cputime64_t cur_wall_time;
	cputime64_t busy_time;

	cur_wall_time = jiffies64_to_cputime64(get_jiffies_64());
	busy_time = cputime64_add(kstat_cpu(cpu).cpustat.user,
			kstat_cpu(cpu).cpustat.system);

	busy_time = cputime64_add(busy_time, kstat_cpu(cpu).cpustat.irq);
	busy_time = cputime64_add(busy_time, kstat_cpu(cpu).cpustat.softirq);
	busy_time = cputime64_add(busy_time, kstat_cpu(cpu).cpustat.steal);
	busy_time = cputime64_add(busy_time, kstat_cpu(cpu).cpustat.nice);

	idle_time = cputime64_sub(cur_wall_time, busy_time);
	if (wall)
		*wall = (cputime64_t)jiffies_to_usecs(cur_wall_time);

	return (cputime64_t)jiffies_to_usecs(idle_time);
}

static inline cputime64_t get_cpu_idle_time(unsigned int cpu, cputime64_t *wall)
{
	u64 idle_time = get_cpu_idle_time_us(cpu, wall);

	if (idle_time == -1ULL)
		return get_cpu_idle_time_jiffy(cpu, wall);

	return idle_time;
}

/* keep track of frequency transitions */
static int
dbs_cpufreq_notifier(struct notifier_block *nb, unsigned long val,
		     void *data)
{
	struct cpufreq_freqs *freq = data;
	struct cpu_dbs_info_s *this_dbs_info = &per_cpu(cs_cpu_dbs_info,
							freq->cpu);

	struct cpufreq_policy *policy;

	if (!this_dbs_info->enable)
		return 0;

	policy = this_dbs_info->cur_policy;

	/*
	 * we only care if our internally tracked freq moves outside
	 * the 'valid' ranges of frequency available to us otherwise
	 * we do not change it
	*/
	if (this_dbs_info->requested_freq > policy->max
			|| this_dbs_info->requested_freq < policy->min)
		this_dbs_info->requested_freq = freq->new;

	return 0;
}

static struct notifier_block dbs_cpufreq_notifier_block = {
	.notifier_call = dbs_cpufreq_notifier
};

/************************** sysfs interface ************************/
static ssize_t show_sampling_rate_min(struct kobject *kobj,
				      struct attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", min_sampling_rate);
}

define_one_global_ro(sampling_rate_min);

/* cpufreq_impulse Governor Tunables */
#define show_one(file_name, object)					\
static ssize_t show_##file_name						\
(struct kobject *kobj, struct attribute *attr, char *buf)		\
{									\
	return sprintf(buf, "%u\n", dbs_tuners_ins.object);		\
}
show_one(sampling_rate, sampling_rate);
show_one(sampling_rate_sleep_multiplier, sampling_rate_sleep_multiplier);	//  added sampling_rate_sleep_multiplier tuneable for early suspend
show_one(sampling_down_factor, sampling_down_factor);				//  reactivated sampling down factor
show_one(sampling_down_max_momentum, sampling_down_max_mom);			//  added Sampling down momentum tuneable
show_one(sampling_down_momentum_sensitivity, sampling_down_mom_sens);		//  added Sampling down momentum tuneable
show_one(up_threshold, up_threshold);
show_one(up_threshold_sleep, up_threshold_sleep);				//  added up_threshold_sleep tuneable for early suspend
show_one(up_threshold_hotplug1, up_threshold_hotplug1);				//  added up_threshold_hotplug1 tuneable for cpu1
show_one(up_threshold_hotplug_freq1, up_threshold_hotplug_freq1);		//  added up_threshold_hotplug_freq1 tuneable for cpu1
#if (MAX_CORES == 4 || MAX_CORES == 8)
show_one(up_threshold_hotplug2, up_threshold_hotplug2);				//  added up_threshold_hotplug2 tuneable for cpu2
show_one(up_threshold_hotplug_freq2, up_threshold_hotplug_freq2);		//  added up_threshold_hotplug_freq2 tuneable for cpu2
show_one(up_threshold_hotplug3, up_threshold_hotplug3);				//  added up_threshold_hotplug3 tuneable for cpu3
show_one(up_threshold_hotplug_freq3, up_threshold_hotplug_freq3);		//  added up_threshold_hotplug_freq3 tuneable for cpu3
#endif
#if (MAX_CORES == 8)
show_one(up_threshold_hotplug4, up_threshold_hotplug4);				//  added up_threshold_hotplug4 tuneable for cpu4
show_one(up_threshold_hotplug_freq4, up_threshold_hotplug_freq4);		//  added up_threshold_hotplug_freq4 tuneable for cpu4
show_one(up_threshold_hotplug5, up_threshold_hotplug5);				//  added up_threshold_hotplug5 tuneable for cpu5
show_one(up_threshold_hotplug_freq5, up_threshold_hotplug_freq5);		//  added up_threshold_hotplug_freq5 tuneable for cpu5
show_one(up_threshold_hotplug6, up_threshold_hotplug6);				//  added up_threshold_hotplug6 tuneable for cpu6
show_one(up_threshold_hotplug_freq6, up_threshold_hotplug_freq6);		//  added up_threshold_hotplug_freq6 tuneable for cpu6
show_one(up_threshold_hotplug7, up_threshold_hotplug7);				//  added up_threshold_hotplug7 tuneable for cpu7
show_one(up_threshold_hotplug_freq7, up_threshold_hotplug_freq7);		//  added up_threshold_hotplug_freq7 tuneable for cpu7
#endif
show_one(down_threshold, down_threshold);
show_one(down_threshold_sleep, down_threshold_sleep);				//  added down_threshold_sleep tuneable for early suspend
show_one(down_threshold_hotplug1, down_threshold_hotplug1);			//  added down_threshold_hotplug1 tuneable for cpu1
show_one(down_threshold_hotplug_freq1, down_threshold_hotplug_freq1);		//  added down_threshold_hotplug_freq1 tuneable for cpu1
#if (MAX_CORES == 4 || MAX_CORES == 8)
show_one(down_threshold_hotplug2, down_threshold_hotplug2);			//  added down_threshold_hotplug2 tuneable for cpu2
show_one(down_threshold_hotplug_freq2, down_threshold_hotplug_freq2);		//  added down_threshold_hotplug_freq2 tuneable for cpu2
show_one(down_threshold_hotplug3, down_threshold_hotplug3);			//  added down_threshold_hotplug3 tuneable for cpu3
show_one(down_threshold_hotplug_freq3, down_threshold_hotplug_freq3);		//  added down_threshold_hotplug_freq3 tuneable for cpu3
#endif
#if (MAX_CORES == 8)
show_one(down_threshold_hotplug4, down_threshold_hotplug4);			//  added down_threshold_hotplug4 tuneable for cpu4
show_one(down_threshold_hotplug_freq4, down_threshold_hotplug_freq4);		//  added down_threshold_hotplug_freq4 tuneable for cpu4
show_one(down_threshold_hotplug5, down_threshold_hotplug5);			//  added down_threshold_hotplug5 tuneable for cpu5
show_one(down_threshold_hotplug_freq5, down_threshold_hotplug_freq5);		//  added down_threshold_hotplug_freq5 tuneable for cpu5
show_one(down_threshold_hotplug6, down_threshold_hotplug6);			//  added down_threshold_hotplug6 tuneable for cpu6
show_one(down_threshold_hotplug_freq6, down_threshold_hotplug_freq6);		//  added down_threshold_hotplug_freq6 tuneable for cpu6
show_one(down_threshold_hotplug7, down_threshold_hotplug7);			//  added down_threshold_hotplug7 tuneable for cpu7
show_one(down_threshold_hotplug_freq7, down_threshold_hotplug_freq7);		//  added down_threshold_hotplug_freq7 tuneable for cpu7
#endif
show_one(ignore_nice_load, ignore_nice);
show_one(freq_step, freq_step);
show_one(freq_step_sleep, freq_step_sleep);					//  added freq_step_sleep tuneable for early suspend
show_one(smooth_up, smooth_up);
show_one(smooth_up_sleep, smooth_up_sleep);					//  added smooth_up_sleep tuneable for early suspend
show_one(hotplug_sleep, hotplug_sleep);						//  added hotplug_sleep tuneable for early suspend
show_one(freq_limit, freq_limit);						//  added freq_limit tuneable
show_one(freq_limit_sleep, freq_limit_sleep);					//  added freq_limit_sleep tuneable for early suspend
show_one(fast_scaling, fast_scaling);						//  added fast_scaling tuneable
show_one(fast_scaling_sleep, fast_scaling_sleep);				//  added fast_scaling_sleep tuneable for early suspend
show_one(grad_up_threshold, grad_up_threshold);					//  added Early demand tuneable grad up threshold
show_one(early_demand, early_demand);						//  added Early demand tuneable master switch
show_one(disable_hotplug, disable_hotplug);					//  added Hotplug switch
show_one(disable_hotplug_sleep, disable_hotplug_sleep);				//  added Hotplug switch for sleep
show_one(hotplug_block_cycles, hotplug_block_cycles);				//  added Hotplug block cycles
show_one(hotplug_idle_threshold, hotplug_idle_threshold);			//  added Hotplug idle threshold
#ifdef ENABLE_LEGACY_MODE
show_one(legacy_mode, legacy_mode);						//  Legacy Mode switch
#endif
#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
show_one(lcdfreq_enable, lcdfreq_enable);					//  added LCDFreq Scaling tuneable master switch
show_one(lcdfreq_kick_in_down_delay, lcdfreq_kick_in_down_delay);		//  added LCDFreq Scaling tuneable kick in down delay
show_one(lcdfreq_kick_in_up_delay, lcdfreq_kick_in_up_delay);			//  added LCDFreq Scaling tuneable kick in up delay
show_one(lcdfreq_kick_in_freq, lcdfreq_kick_in_freq);				//  added LCDFreq Scaling tuneable kick in freq
show_one(lcdfreq_kick_in_cores, lcdfreq_kick_in_cores);				//  added LCDFreq Scaling tuneable kick in cores
#endif

//  added tuneable for Sampling down momentum -> possible values: 0 (disable) to MAX_SAMPLING_DOWN_FACTOR, if not set default is 0
static ssize_t store_sampling_down_max_momentum(struct kobject *a,
		struct attribute *b, const char *buf, size_t count)
{
	unsigned int input, j;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > MAX_SAMPLING_DOWN_FACTOR -
	dbs_tuners_ins.sampling_down_factor || input < 0)
	return -EINVAL;

	dbs_tuners_ins.sampling_down_max_mom = input;
	orig_sampling_down_max_mom = dbs_tuners_ins.sampling_down_max_mom;

	/*  Reset sampling down factor to default if momentum was disabled */
	if (dbs_tuners_ins.sampling_down_max_mom == 0)
	dbs_tuners_ins.sampling_down_factor = DEF_SAMPLING_DOWN_FACTOR;

	/* Reset momentum_adder and reset down sampling multiplier in case momentum was disabled */
	for_each_online_cpu(j) {
	    struct cpu_dbs_info_s *dbs_info;
	    dbs_info = &per_cpu(cs_cpu_dbs_info, j);
	    dbs_info->momentum_adder = 0;
	    if (dbs_tuners_ins.sampling_down_max_mom == 0)
	    dbs_info->rate_mult = 1;
	}

return count;
}

//  added tuneable for Sampling down momentum -> possible values: 1 to MAX_SAMPLING_DOWN_SENSITIVITY, if not set default is 50
static ssize_t store_sampling_down_momentum_sensitivity(struct kobject *a,
			struct attribute *b, const char *buf, size_t count)
{
	unsigned int input, j;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > MAX_SAMPLING_DOWN_MOMENTUM_SENSITIVITY || input < 1)
	return -EINVAL;

	dbs_tuners_ins.sampling_down_mom_sens = input;

	/* Reset momentum_adder */
	for_each_online_cpu(j) {
	    struct cpu_dbs_info_s *dbs_info;
	    dbs_info = &per_cpu(cs_cpu_dbs_info, j);
	    dbs_info->momentum_adder = 0;
	}

return count;
}

//  Sampling down factor (reactivated) added reset loop for momentum functionality -> possible values: 1 (disabled) to MAX_SAMPLING_DOWN_FACTOR, if not set default is 1
static ssize_t store_sampling_down_factor(struct kobject *a,
					  struct attribute *b,
					  const char *buf, size_t count)
{
	unsigned int input, j;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > MAX_SAMPLING_DOWN_FACTOR || input < 1)
		return -EINVAL;

	dbs_tuners_ins.sampling_down_factor = input;

	/*  Reset down sampling multiplier in case it was active */
	for_each_online_cpu(j) {
	    struct cpu_dbs_info_s *dbs_info;
	    dbs_info = &per_cpu(cs_cpu_dbs_info, j);
	    dbs_info->rate_mult = 1;
	}

	return count;
}

static ssize_t store_sampling_rate(struct kobject *a, struct attribute *b,
				   const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1)
		return -EINVAL;

	dbs_tuners_ins.sampling_rate = max(input, min_sampling_rate);

	return count;
}

//  added tuneable -> possible values: 1 to 4, if not set default is 2
static ssize_t store_sampling_rate_sleep_multiplier(struct kobject *a, struct attribute *b,
				   const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > MAX_SAMPLING_RATE_SLEEP_MULTIPLIER || input < 1)
		return -EINVAL;

	dbs_tuners_ins.sampling_rate_sleep_multiplier = input;
	return count;
}

static ssize_t store_up_threshold(struct kobject *a, struct attribute *b,
				  const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > 100 ||
			input <= dbs_tuners_ins.down_threshold)
		return -EINVAL;

	dbs_tuners_ins.up_threshold = input;
	return count;
}

//  added tuneble -> possible values: range from above down_threshold_sleep value up to 100, if not set default is 90
static ssize_t store_up_threshold_sleep(struct kobject *a, struct attribute *b,
				  const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > 100 ||
			input <= dbs_tuners_ins.down_threshold_sleep)
		return -EINVAL;

	dbs_tuners_ins.up_threshold_sleep = input;
	return count;
}

//  also use definitions for other hotplug tunables

#define store_up_threshold_hotplug(name,core)							\
static ssize_t store_up_threshold_hotplug##name							\
(struct kobject *a, struct attribute *b, const char *buf, size_t count)				\
{												\
	unsigned int input;									\
	int ret;										\
	ret = sscanf(buf, "%u", &input);							\
												\
	if (hotplug_thresholds_tuneable[core] == 0) {						\
												\
	if (ret != 1 || input > 100								\
	|| (input <= dbs_tuners_ins.down_threshold_hotplug##name && input != 0))		\
		return -EINVAL;									\
												\
	dbs_tuners_ins.up_threshold_hotplug##name = input;					\
	hotplug_thresholds[0][core] = input;							\
												\
	} else {										\
	if (ret != 1 || input < 1 || input > 100)						\
		return -EINVAL;									\
	dbs_tuners_ins.up_threshold_hotplug##name = input;					\
	hotplug_thresholds[0][core] = input;							\
	hotplug_thresholds_tuneable[core] = 0;						\
	}											\
	return count;										\
}												\

#define store_down_threshold_hotplug(name,core)							\
static ssize_t store_down_threshold_hotplug##name						\
(struct kobject *a, struct attribute *b, const char *buf, size_t count)				\
{												\
	unsigned int input;									\
	int ret;										\
	ret = sscanf(buf, "%u", &input);							\
												\
	if (hotplug_thresholds_tuneable[core] == 0) {					\
												\
	if (ret != 1 || input < 11 || input > 100						\
	|| input >= dbs_tuners_ins.up_threshold_hotplug##name)					\
		return -EINVAL;									\
	    dbs_tuners_ins.down_threshold_hotplug##name = input;				\
	    hotplug_thresholds[1][core] = input;						\
	} else {										\
	    if (ret != 1 || input < 11 || input > 100)						\
		return -EINVAL;									\
	    dbs_tuners_ins.down_threshold_hotplug##name = input;				\
	    hotplug_thresholds[1][core] = input;						\
	    hotplug_thresholds_tuneable[core] = 0;						\
	}											\
	return count;										\
}

//  added tuneable -> possible values: 0 to disable core, range from down_threshold up to 100, if not set default is 68
store_up_threshold_hotplug(1,0);
store_down_threshold_hotplug(1,0);
#if (MAX_CORES == 4 || MAX_CORES == 8)
store_up_threshold_hotplug(2,1);
store_down_threshold_hotplug(2,1);
store_up_threshold_hotplug(3,2);
store_down_threshold_hotplug(3,2);
#endif
#if (MAX_CORES == 8)
store_up_threshold_hotplug(4,3);
store_down_threshold_hotplug(4,3);
store_up_threshold_hotplug(5,4);
store_down_threshold_hotplug(5,4);
store_up_threshold_hotplug(6,5);
store_down_threshold_hotplug(6,5);
store_up_threshold_hotplug(7,6);
store_down_threshold_hotplug(7,6);
#endif

static ssize_t store_down_threshold(struct kobject *a, struct attribute *b,
				    const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	/* cannot be lower than 11 otherwise freq will not fall */
	if (ret != 1 || input < 11 || input > 100 ||
			input >= dbs_tuners_ins.up_threshold)
		return -EINVAL;

	dbs_tuners_ins.down_threshold = input;
	return count;
}

//  added tuneable -> possible values: range from 11 to up_threshold_sleep but not up_threshold_sleep, if not set default is 44
static ssize_t store_down_threshold_sleep(struct kobject *a, struct attribute *b,
				    const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	/* cannot be lower than 11 otherwise freq will not fall */
	if (ret != 1 || input < 11 || input > 100 ||
			input >= dbs_tuners_ins.up_threshold_sleep)
		return -EINVAL;

	dbs_tuners_ins.down_threshold_sleep = input;
	return count;
}

static ssize_t store_ignore_nice_load(struct kobject *a, struct attribute *b,
				      const char *buf, size_t count)
{
	unsigned int input;
	int ret;

	unsigned int j;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
		return -EINVAL;

	if (input > 1)
		input = 1;

	if (input == dbs_tuners_ins.ignore_nice) { /* nothing to do */
		return count;
	}

	dbs_tuners_ins.ignore_nice = input;

	/* we need to re-evaluate prev_cpu_idle */
	for_each_online_cpu(j) {
		struct cpu_dbs_info_s *dbs_info;
		dbs_info = &per_cpu(cs_cpu_dbs_info, j);
		dbs_info->prev_cpu_idle = get_cpu_idle_time(j,
						&dbs_info->prev_cpu_wall);
		if (dbs_tuners_ins.ignore_nice)
			dbs_info->prev_cpu_nice = kstat_cpu(j).cpustat.nice;

	}
	return count;
}

static ssize_t store_freq_step(struct kobject *a, struct attribute *b,
			       const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1)
		return -EINVAL;

	if (input > 100)
		input = 100;

	/* no need to test here if freq_step is zero as the user might actually
	 * want this, they would be crazy though :) */
	dbs_tuners_ins.freq_step = input;
	return count;
}

/*
 *  added tuneable -> possible values: range from 0 to 100, if not set default is 5 -> value 0 will stop freq scaling and hold on actual freq
 * value 100 will directly jump up/down to limits like ondemand governor
 */
static ssize_t store_freq_step_sleep(struct kobject *a, struct attribute *b,
			       const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1)
		return -EINVAL;

	if (input > 100)
		input = 100;

	/* no need to test here if freq_step is zero as the user might actually
	 * want this, they would be crazy though :) */
	dbs_tuners_ins.freq_step_sleep = input;
	return count;
}

static ssize_t store_smooth_up(struct kobject *a,
					  struct attribute *b,
					  const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > 100 || input < 1)
		return -EINVAL;

	dbs_tuners_ins.smooth_up = input;
	return count;
}

//  added tuneable -> possible values: range from 1 to 100, if not set default is 100
static ssize_t store_smooth_up_sleep(struct kobject *a,
					  struct attribute *b,
					  const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > 100 || input < 1)
		return -EINVAL;

	dbs_tuners_ins.smooth_up_sleep = input;
	return count;
}

/*
 *  added tuneable -> possible values: 0 do not touch the hotplug values on early suspend,
 * input value 1 to MAX_CORES - 1 equals cores to run at early suspend, if not set default is 0 (=all cores enabled)
 */
static ssize_t store_hotplug_sleep(struct kobject *a,
					  struct attribute *b,
					  const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input >= num_possible_cpus() || (input < 0 && input != 0))
	return -EINVAL;

	dbs_tuners_ins.hotplug_sleep = input;
	return count;
}

//  added tuneable -> possible values: 0 disable, system table freq->min to freq->max in khz -> freqency soft-limit, if not set default is 0
//  updated : possible values now depend on the system frequency table only
static ssize_t store_freq_limit(struct kobject *a,
					  struct attribute *b,
					  const char *buf, size_t count)
{
	unsigned int input;
	struct cpufreq_frequency_table *table;	//  Use system frequency table
	int ret;
	int i=0;

	ret = sscanf(buf, "%u", &input);

	if (ret != 1)
		return -EINVAL;

	table = cpufreq_frequency_get_table(0);	//  Get system frequency table

	if (!table)
		return -EINVAL;

	if (input == 0) {
	     max_scaling_freq_soft = max_scaling_freq_hard;
		if (freq_table_order == 1)			//  if descending ordered table is used
		    limit_table_start = max_scaling_freq_soft;	//  we should use the actual scaling soft limit value as search start point
		else
		    limit_table_end = table[freq_table_size].frequency; //  set search end point to max freq when using ascending table
	     dbs_tuners_ins.freq_limit = input;
	     return count;
	}

	if (input > table[max_scaling_freq_hard].frequency) {	 //  Allow only frequencies below or equal to hard max limit
		return -EINVAL;
	} else {
		for (i = 0; (table[i].frequency != CPUFREQ_TABLE_END); i++)
			if (table[i].frequency == input) {
				max_scaling_freq_soft = i;
				if (freq_table_order == 1)			//  if descending ordered table is used
				    limit_table_start = max_scaling_freq_soft;	//  we should use the actual scaling soft limit value as search start point
				else
				    limit_table_end = table[i].frequency;	//  set search end point to max soft freq limit when using ascenting table
				dbs_tuners_ins.freq_limit = input;
				return count;
			}
	}

	return -EINVAL;
}

//  added tuneable -> possible values: 0 disable, system table freq->min to freq->max in khz -> freqency soft-limit, if not set default is 0
//  updated : possible values now depend on the system frequency table only
static ssize_t store_freq_limit_sleep(struct kobject *a,
					  struct attribute *b,
					  const char *buf, size_t count)
{
	unsigned int input;
	struct cpufreq_frequency_table *table;	//  Use system frequency table
	int ret;
	int i=0;

	ret = sscanf(buf, "%u", &input);

	if (ret != 1)
		return -EINVAL;

	if (input == 0) {
	     dbs_tuners_ins.freq_limit_sleep = input;
	     return count;
	}

	table = cpufreq_frequency_get_table(0);	//  Get system frequency table

	if (!table) {
		return -EINVAL;
	} else if (input > table[max_scaling_freq_hard].frequency) {	 //  Allow only frequencies below or equal to hard max
		return -EINVAL;
	} else {
		for (i = 0; (table[i].frequency != CPUFREQ_TABLE_END); i++)
			if (table[i].frequency == input) {
			    dbs_tuners_ins.freq_limit_sleep = input;
			    return count;
			}
	}

	return -EINVAL;
}

//  added tuneable -> possible values: 0 disable, 1-4 number of scaling jumps only for upscaling, 5-8 equivalent to 1-4 for up and down scaling, if not set default is 0
static ssize_t store_fast_scaling(struct kobject *a,
					  struct attribute *b,
					  const char *buf, size_t count)
{
	unsigned int input;
	int ret;

	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > 8 || input < 0)
		return -EINVAL;

	dbs_tuners_ins.fast_scaling = input;

	if (input > 4) {
	    scaling_mode_up   = input - 4;	//  fast scaling up
	    scaling_mode_down = input - 4;	//  fast scaling down

	} else {
	    scaling_mode_up   = input;		//  fast scaling up only
	    scaling_mode_down = 0;
	}
	return count;
}

//  added tuneable -> possible values: 0 disable, 1-4 number of scaling jumps only for upscaling, 5-8 equivalent to 1-4 for up and down scaling, if not set default is 0
static ssize_t store_fast_scaling_sleep(struct kobject *a,
					  struct attribute *b,
					  const char *buf, size_t count)
{
	unsigned int input;
	int ret;

	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > 8 || input < 0)
		return -EINVAL;

	dbs_tuners_ins.fast_scaling_sleep = input;
	return count;
}

//  LCDFreq Scaling - added tuneable (Master Switch) -> possible values: 0 to disable, any value above 0 to enable, if not set default is 0
#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
static ssize_t store_lcdfreq_enable(struct kobject *a, struct attribute *b,
					    const char *buf, size_t count)
{
	unsigned int input;
	int ret;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
	return -EINVAL;

	if (input > 0) {
		dbs_tuners_ins.lcdfreq_enable = true;
	}
	else {
		dbs_tuners_ins.lcdfreq_enable = false;
		// Set screen to 60Hz when stopping to switch
		lcdfreq_lock_current = 0;
		_lcdfreq_lock(lcdfreq_lock_current);
	}
	return count;
}

//  LCDFreq Scaling - added tuneable (Down Delay) -> number of samples to wait till switching to 40hz, possible range not specified, if not set default is 20
static ssize_t store_lcdfreq_kick_in_down_delay(struct kobject *a, struct attribute *b,
							const char *buf, size_t count)
{
	unsigned int input;
	int ret;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1 && input < 0)
	return -EINVAL;

	dbs_tuners_ins.lcdfreq_kick_in_down_delay = input;
	dbs_tuners_ins.lcdfreq_kick_in_down_left =
	dbs_tuners_ins.lcdfreq_kick_in_down_delay;
	return count;
}

//  LCDFreq Scaling - added tuneable (Up Delay) -> number of samples to wait till switching to 40hz, possible range not specified, if not set default is 50
static ssize_t store_lcdfreq_kick_in_up_delay(struct kobject *a, struct attribute *b,
							const char *buf, size_t count)
{
	unsigned int input;
	int ret;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1 && input < 0)
	return -EINVAL;

	dbs_tuners_ins.lcdfreq_kick_in_up_delay = input;
	dbs_tuners_ins.lcdfreq_kick_in_up_left =
	dbs_tuners_ins.lcdfreq_kick_in_up_delay;
	return count;
}

//  LCDFreq Scaling - added tuneable (Frequency Threshold) -> frequency from where to start switching LCDFreq, possible values are all valid frequencies up to actual scaling limit, if not set default is 500000
//  updated : possible values now depend on the system frequency table only
static ssize_t store_lcdfreq_kick_in_freq(struct kobject *a, struct attribute *b,
						    const char *buf, size_t count)
{
	unsigned int input;
	struct cpufreq_frequency_table *table;	//  Use system frequency table
	int ret;
	int i=0;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
		return -EINVAL;

	table = cpufreq_frequency_get_table(0);	//  Get system frequency table

	if (!table) {
		return -EINVAL;
	} else if (input > table[max_scaling_freq_hard].frequency) {	 //  Allow only frequencies below or equal to hard max
		return -EINVAL;
	} else {
		for (i = 0; (table[i].frequency != CPUFREQ_TABLE_END); i++)
			if (table[i].frequency == input) {
				dbs_tuners_ins.lcdfreq_kick_in_freq = input;
				return count;
			}
	}

	return -EINVAL;
}

//  LCDFreq Scaling - added tuneable (Online Cores Threshold) -> amount of cores which have to be online before switching LCDFreq (useable in combination with Freq Threshold), possible values 0-4, if not set default is 0
static ssize_t store_lcdfreq_kick_in_cores(struct kobject *a, struct attribute *b,
						    const char *buf, size_t count)
{
	unsigned int input;
	int ret;

	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > num_possible_cpus() || (input < 0 && input != 0))
	return -EINVAL;

	dbs_tuners_ins.lcdfreq_kick_in_cores = input;
	return count;
}
#endif

//  Early demand - added tuneable grad up threshold -> possible values: from 11 to 100, if not set default is 50
static ssize_t store_grad_up_threshold(struct kobject *a, struct attribute *b,
						const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > 100 || input < 11)
	return -EINVAL;

	dbs_tuners_ins.grad_up_threshold = input;
	return count;
}

//  Early demand - added tuneable master switch -> possible values: 0 to disable, any value above 0 to enable, if not set default is 0
static ssize_t store_early_demand(struct kobject *a, struct attribute *b,
					    const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1)
	return -EINVAL;

	dbs_tuners_ins.early_demand = !!input;
	return count;
}

//  added tuneable hotplug switch -> possible values: 0 to disable, any value above 0 to enable, if not set default is 0
static ssize_t store_disable_hotplug(struct kobject *a, struct attribute *b,
					    const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	int i=0;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
	return -EINVAL;

	if (input > 0) {
		dbs_tuners_ins.disable_hotplug = true;
			for (i = 1; i < num_possible_cpus(); i++) {		//  enable all offline cores
			if (!cpu_online(i))
			cpu_up(i);
			}
	} else {
		dbs_tuners_ins.disable_hotplug = false;
	}
	return count;
}

//  added tuneable hotplug switch for early supend -> possible values: 0 to disable, any value above 0 to enable, if not set default is 0
static ssize_t store_disable_hotplug_sleep(struct kobject *a, struct attribute *b,
					    const char *buf, size_t count)
{
	unsigned int input;
	int ret;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
	return -EINVAL;

	if (input > 0)
		dbs_tuners_ins.disable_hotplug_sleep = true;
	else
		dbs_tuners_ins.disable_hotplug_sleep = false;
	return count;
}

//  added tuneable hotplug block cycles -> possible values: 0 to disable, any value above 0 to enable, if not set default is 0
static ssize_t store_hotplug_block_cycles(struct kobject *a, struct attribute *b,
					    const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (input < 0)
	return -EINVAL;

	if (input == 0)
	hotplug_up_block_cycles = 0;

	dbs_tuners_ins.hotplug_block_cycles = input;

	return count;
}

#ifdef ENABLE_LEGACY_MODE
//  added tuneable for Legacy Mode -> possible values: 0 to disable, any value above 0 to enable, if not set default is 0
static ssize_t store_legacy_mode(struct kobject *a, struct attribute *b,
					    const char *buf, size_t count)
{
	unsigned int input;
	int ret;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
	return -EINVAL;

	if (input > 0)
		dbs_tuners_ins.legacy_mode = true;
	 else
		dbs_tuners_ins.legacy_mode = false;
	return count;
}
#endif

//  added tuneable hotplug idle threshold -> possible values: range from 0 disabled to 100, if not set default is 0
static ssize_t store_hotplug_idle_threshold(struct kobject *a, struct attribute *b,
				    const char *buf, size_t count)
{
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if ((ret != 1 || input < 0 || input > 100) && input != 0)
		return -EINVAL;

	dbs_tuners_ins.hotplug_idle_threshold = input;
	return count;
}

//  add hotplug up/down threshold sysfs store interface

#define store_up_threshold_hotplug_freq(name,core)						\
static ssize_t store_up_threshold_hotplug_freq##name						\
(struct kobject *a, struct attribute *b, const char *buf, size_t count)				\
{												\
	unsigned int input;									\
	struct cpufreq_frequency_table *table;							\
	int ret;										\
	int i=0;										\
												\
	ret = sscanf(buf, "%u", &input);							\
	if (ret != 1)										\
		return -EINVAL;									\
												\
	if (input == 0) {									\
		dbs_tuners_ins.up_threshold_hotplug_freq##name = input;				\
		hotplug_thresholds_freq[0][core] = input;					\
		return count;									\
	}											\
												\
	if (input <= dbs_tuners_ins.down_threshold_hotplug_freq##name				\
		&& dbs_tuners_ins.down_threshold_hotplug_freq##name != 0)			\
		return -EINVAL;									\
												\
	table = cpufreq_frequency_get_table(0);							\
												\
	if (!table) {										\
		return -EINVAL;									\
	} else if (input > table[max_scaling_freq_hard].frequency) {				\
		return -EINVAL;									\
	} else {										\
		for (i = 0; (table[i].frequency != CPUFREQ_TABLE_END); i++)			\
			if (table[i].frequency == input) {					\
				dbs_tuners_ins.up_threshold_hotplug_freq##name = input;		\
				hotplug_thresholds_freq[0][core] = input;			\
				return count;							\
			}									\
	}											\
												\
	return -EINVAL;										\
}

#define store_down_threshold_hotplug_freq(name,core)						\
static ssize_t store_down_threshold_hotplug_freq##name						\
(struct kobject *a, struct attribute *b, const char *buf, size_t count)				\
{												\
	unsigned int input;									\
	struct cpufreq_frequency_table *table;							\
	int ret;										\
	int i=0;										\
												\
	ret = sscanf(buf, "%u", &input);							\
	if (ret != 1)										\
		return -EINVAL;									\
												\
	if (input == 0) {									\
		dbs_tuners_ins.down_threshold_hotplug_freq##name = input;			\
		hotplug_thresholds_freq[1][core] = input;					\
		return count;									\
	}											\
												\
	if (input >= dbs_tuners_ins.up_threshold_hotplug_freq##name				\
		&& dbs_tuners_ins.up_threshold_hotplug_freq##name != 0)				\
		return -EINVAL;									\
												\
	table = cpufreq_frequency_get_table(0);							\
												\
	if (!table) {										\
		return -EINVAL;									\
	} else if (input > table[max_scaling_freq_hard].frequency) {				\
		return -EINVAL;									\
	} else {										\
		for (i = 0; (table[i].frequency != CPUFREQ_TABLE_END); i++)			\
			if (table[i].frequency == input) {					\
				dbs_tuners_ins.down_threshold_hotplug_freq##name = input;	\
				hotplug_thresholds_freq[1][core] = input;			\
				return count;							\
			}									\
	}											\
												\
	return -EINVAL;										\
}

store_up_threshold_hotplug_freq(1,0);
store_down_threshold_hotplug_freq(1,0);
#if (MAX_CORES == 4 || MAX_CORES == 8)
store_up_threshold_hotplug_freq(2,1);
store_down_threshold_hotplug_freq(2,1);
store_up_threshold_hotplug_freq(3,2);
store_down_threshold_hotplug_freq(3,2);
#endif
#if (MAX_CORES == 8)
store_up_threshold_hotplug_freq(4,3);
store_down_threshold_hotplug_freq(4,3);
store_up_threshold_hotplug_freq(5,4);
store_down_threshold_hotplug_freq(5,4);
store_up_threshold_hotplug_freq(6,5);
store_down_threshold_hotplug_freq(6,5);
store_up_threshold_hotplug_freq(7,6);
store_down_threshold_hotplug_freq(7,6);
#endif

define_one_global_rw(sampling_rate);
define_one_global_rw(sampling_rate_sleep_multiplier);		//  added tuneable
define_one_global_rw(sampling_down_factor);			//  Sampling down factor (reactived)
define_one_global_rw(sampling_down_max_momentum);		//  Sampling down momentum tuneable
define_one_global_rw(sampling_down_momentum_sensitivity);	//  Sampling down momentum tuneable
define_one_global_rw(up_threshold);
define_one_global_rw(up_threshold_sleep);			//  added tuneable
define_one_global_rw(up_threshold_hotplug1);			//  added tuneable
define_one_global_rw(up_threshold_hotplug_freq1);		//  added tuneable
#if (MAX_CORES == 4 || MAX_CORES == 8)
define_one_global_rw(up_threshold_hotplug2);			//  added tuneable
define_one_global_rw(up_threshold_hotplug_freq2);		//  added tuneable
define_one_global_rw(up_threshold_hotplug3);			//  added tuneable
define_one_global_rw(up_threshold_hotplug_freq3);		//  added tuneable
#endif
#if (MAX_CORES == 8)
define_one_global_rw(up_threshold_hotplug4);			//  added tuneable
define_one_global_rw(up_threshold_hotplug_freq4);		//  added tuneable
define_one_global_rw(up_threshold_hotplug5);			//  added tuneable
define_one_global_rw(up_threshold_hotplug_freq5);		//  added tuneable
define_one_global_rw(up_threshold_hotplug6);			//  added tuneable
define_one_global_rw(up_threshold_hotplug_freq6);		//  added tuneable
define_one_global_rw(up_threshold_hotplug7);			//  added tuneable
define_one_global_rw(up_threshold_hotplug_freq7);		//  added tuneable
#endif
define_one_global_rw(down_threshold);
define_one_global_rw(down_threshold_sleep);			//  added tuneable
define_one_global_rw(down_threshold_hotplug1);			//  added tuneable
define_one_global_rw(down_threshold_hotplug_freq1);		//  added tuneable
#if (MAX_CORES == 4 || MAX_CORES == 8)
define_one_global_rw(down_threshold_hotplug2);			//  added tuneable
define_one_global_rw(down_threshold_hotplug_freq2);		//  added tuneable
define_one_global_rw(down_threshold_hotplug3);			//  added tuneable
define_one_global_rw(down_threshold_hotplug_freq3);		//  added tuneable
#endif
#if (MAX_CORES == 8)
define_one_global_rw(down_threshold_hotplug4);			//  added tuneable
define_one_global_rw(down_threshold_hotplug_freq4);		//  added tuneable
define_one_global_rw(down_threshold_hotplug5);			//  added tuneable
define_one_global_rw(down_threshold_hotplug_freq5);		//  added tuneable
define_one_global_rw(down_threshold_hotplug6);			//  added tuneable
define_one_global_rw(down_threshold_hotplug_freq6);		//  added tuneable
define_one_global_rw(down_threshold_hotplug7);			//  added tuneable
define_one_global_rw(down_threshold_hotplug_freq7);		//  added tuneable
#endif
define_one_global_rw(ignore_nice_load);
define_one_global_rw(freq_step);
define_one_global_rw(freq_step_sleep);				//  added tuneable
define_one_global_rw(smooth_up);
define_one_global_rw(smooth_up_sleep);				//  added tuneable
define_one_global_rw(hotplug_sleep);				//  added tuneable
define_one_global_rw(freq_limit);				//  added tuneable
define_one_global_rw(freq_limit_sleep);				//  added tuneable
define_one_global_rw(fast_scaling);				//  added tuneable
define_one_global_rw(fast_scaling_sleep);			//  added tuneable
define_one_global_rw(grad_up_threshold);			//  Early demand tuneable
define_one_global_rw(early_demand);				//  Early demand tuneable
define_one_global_rw(disable_hotplug);				//  Hotplug switch
define_one_global_rw(disable_hotplug_sleep);			//  Hotplug switch for sleep
define_one_global_rw(hotplug_block_cycles);			//  Hotplug block cycles
define_one_global_rw(hotplug_idle_threshold);			//  Hotplug idle threshold
#ifdef ENABLE_LEGACY_MODE
define_one_global_rw(legacy_mode);				//  Legacy Mode switch
#endif
#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
define_one_global_rw(lcdfreq_enable);				//  LCDFreq Scaling tuneable
define_one_global_rw(lcdfreq_kick_in_down_delay);		//  LCDFreq Scaling tuneable
define_one_global_rw(lcdfreq_kick_in_up_delay);			//  LCDFreq Scaling tuneable
define_one_global_rw(lcdfreq_kick_in_freq);			//  LCDFreq Scaling tuneable
define_one_global_rw(lcdfreq_kick_in_cores);			//  LCDFreq Scaling tuneable
#endif

//  add version info tunable
static ssize_t show_version(struct device *dev, struct device_attribute *attr, char *buf) {

    return sprintf(buf, "%s\n", IMPULSE_VERSION);

    }

    static DEVICE_ATTR(version, S_IRUGO , show_version, NULL);

static struct attribute *dbs_attributes[] = {
	&sampling_rate_min.attr,
	&sampling_rate.attr,
	&sampling_rate_sleep_multiplier.attr,			//  added tuneable
	&sampling_down_factor.attr,
	&sampling_down_max_momentum.attr,			//  Sampling down momentum tuneable
	&sampling_down_momentum_sensitivity.attr,		//  Sampling down momentum tuneable
	&up_threshold_hotplug1.attr,				//  added tuneable
	&up_threshold_hotplug_freq1.attr,			//  added tuneable
#if (MAX_CORES == 4 || MAX_CORES == 8)
	&up_threshold_hotplug2.attr,				//  added tuneable
	&up_threshold_hotplug_freq2.attr,			//  added tuneable
	&up_threshold_hotplug3.attr,				//  added tuneable
	&up_threshold_hotplug_freq3.attr,			//  added tuneable
#endif
#if (MAX_CORES == 8)
	&up_threshold_hotplug4.attr,				//  added tuneable
	&up_threshold_hotplug_freq4.attr,			//  added tuneable
	&up_threshold_hotplug5.attr,				//  added tuneable
	&up_threshold_hotplug_freq5.attr,			//  added tuneable
	&up_threshold_hotplug6.attr,				//  added tuneable
	&up_threshold_hotplug_freq6.attr,			//  added tuneable
	&up_threshold_hotplug7.attr,				//  added tuneable
	&up_threshold_hotplug_freq7.attr,			//  added tuneable
#endif
	&down_threshold.attr,
	&down_threshold_sleep.attr,				//  added tuneable
	&down_threshold_hotplug1.attr,				//  added tuneable
	&down_threshold_hotplug_freq1.attr,			//  added tuneable
#if (MAX_CORES == 4 || MAX_CORES == 8)
	&down_threshold_hotplug2.attr,				//  added tuneable
	&down_threshold_hotplug_freq2.attr,			//  added tuneable
	&down_threshold_hotplug3.attr,				//  added tuneable
	&down_threshold_hotplug_freq3.attr,			//  added tuneable
#endif
#if (MAX_CORES == 8)
	&down_threshold_hotplug4.attr,				//  added tuneable
	&down_threshold_hotplug_freq4.attr,			//  added tuneable
	&down_threshold_hotplug5.attr,				//  added tuneable
	&down_threshold_hotplug_freq5.attr,			//  added tuneable
	&down_threshold_hotplug6.attr,				//  added tuneable
	&down_threshold_hotplug_freq6.attr,			//  added tuneable
	&down_threshold_hotplug7.attr,				//  added tuneable
	&down_threshold_hotplug_freq7.attr,			//  added tuneable
#endif
	&ignore_nice_load.attr,
	&freq_step.attr,
	&freq_step_sleep.attr,					//  added tuneable
	&smooth_up.attr,
	&smooth_up_sleep.attr,					//  added tuneable
	&up_threshold.attr,
	&up_threshold_sleep.attr,				//  added tuneable
	&hotplug_sleep.attr,					//  added tuneable
	&freq_limit.attr,					//  added tuneable
	&freq_limit_sleep.attr,					//  added tuneable
	&fast_scaling.attr,					//  added tuneable
	&fast_scaling_sleep.attr,				//  added tuneable
	&grad_up_threshold.attr,				//  Early demand tuneable
	&early_demand.attr,					//  Early demand tuneable
	&disable_hotplug.attr,					//  Hotplug switch
	&disable_hotplug_sleep.attr,				//  Hotplug switch sleep
	&hotplug_block_cycles.attr,				//  Hotplug block cycles
	&hotplug_idle_threshold.attr,				//  Hotplug idle threshold
#ifdef ENABLE_LEGACY_MODE
	&legacy_mode.attr,					//  Legacy Mode switch
#endif
#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
	&lcdfreq_enable.attr,					//  LCD Freq Scaling tuneable
	&lcdfreq_kick_in_down_delay.attr,			//  LCD Freq Scaling tuneable
	&lcdfreq_kick_in_up_delay.attr,				//  LCD Freq Scaling tuneable
	&lcdfreq_kick_in_freq.attr,				//  LCD Freq Scaling tuneable
	&lcdfreq_kick_in_cores.attr,				//  LCD Freq Scaling tuneable
#endif
	&dev_attr_version.attr,					//  impulse version
	NULL
};

static struct attribute_group dbs_attr_group = {
	.attrs = dbs_attributes,
	.name = "impulse",
};

/************************** sysfs end ************************/

static void dbs_check_cpu(struct cpu_dbs_info_s *this_dbs_info)
{
	unsigned int load = 0;
	unsigned int max_load = 0;
	int boost_freq = 0;					//  Early demand boost freq switch
	struct cpufreq_policy *policy;
	unsigned int j;

	policy = this_dbs_info->cur_policy;

	/*
	 *  Frequency Limit: we try here at a verly early stage to limit freqencies above limit by setting the current target_freq to freq_limit.
	 * This could be for example wakeup or touchboot freqencies which could be above the limit and are out of governors control.
	 * This can not avoid the incrementation to these frequencies but should bring it down again earlier. Not sure if that is
	 * a good way to do that or if its realy working. Just an idea - maybe a remove-candidate!
	 */
	if (dbs_tuners_ins.freq_limit != 0 && policy->cur > dbs_tuners_ins.freq_limit)
	__cpufreq_driver_target(policy, dbs_tuners_ins.freq_limit, CPUFREQ_RELATION_L);

	/*
	 * Every sampling_rate, we check, if current idle time is less than 20%
	 * (default), then we try to increase frequency. Every sampling_rate *
	 * sampling_down_factor, we check, if current idle time is more than 80%
	 * (default), then we try to decrease frequency.
	 *
	 * Any frequency increase takes it to the maximum frequency.
	 * Frequency reduction happens at minimum steps of
	 * 5% (default) of maximum frequency
	 */

	/* Get Absolute Load */
	for_each_cpu(j, policy->cpus) {
		struct cpu_dbs_info_s *j_dbs_info;
		cputime64_t cur_wall_time, cur_idle_time;
		unsigned int idle_time, wall_time;

		j_dbs_info = &per_cpu(cs_cpu_dbs_info, j);

		cur_idle_time = get_cpu_idle_time(j, &cur_wall_time);

		wall_time = (unsigned int) cputime64_sub(cur_wall_time,
				j_dbs_info->prev_cpu_wall);
		j_dbs_info->prev_cpu_wall = cur_wall_time;

		idle_time = (unsigned int) cputime64_sub(cur_idle_time,
				j_dbs_info->prev_cpu_idle);
		j_dbs_info->prev_cpu_idle = cur_idle_time;

		if (dbs_tuners_ins.ignore_nice) {
			cputime64_t cur_nice;
			unsigned long cur_nice_jiffies;

			cur_nice = cputime64_sub(kstat_cpu(j).cpustat.nice,
					 j_dbs_info->prev_cpu_nice);
			/*
			 * Assumption: nice time between sampling periods will
			 * be less than 2^32 jiffies for 32 bit sys
			 */
			cur_nice_jiffies = (unsigned long)
					cputime64_to_jiffies64(cur_nice);

			j_dbs_info->prev_cpu_nice = kstat_cpu(j).cpustat.nice;
			idle_time += jiffies_to_usecs(cur_nice_jiffies);
		}

		if (unlikely(!wall_time || wall_time < idle_time))
			continue;

		load = 100 * (wall_time - idle_time) / wall_time;

		if (load > max_load) {
			max_load = load;
			cur_load = load; //  current load for hotplugging functions
		}

		cur_freq = policy->cur;  //  store current frequency for hotplugging frequency thresholds

	/*
	 * Early demand by stratosk
	 * Calculate the gradient of load_freq. If it is too steep we assume
	 * that the load will go over up_threshold in next iteration(s) and
	 * we increase the frequency immediately
	 */
	if (dbs_tuners_ins.early_demand) {
               if (max_load > this_dbs_info->prev_load &&
               (max_load - this_dbs_info->prev_load >
               dbs_tuners_ins.grad_up_threshold))
                  boost_freq = 1;
           this_dbs_info->prev_load = max_load;
           }
	}

	/*
	 * reduction of possible deadlocks - we try here to avoid deadlocks due to double locking from hotplugging and timer mutex
	 * during start/stop/limit events. to be "sure" we skip here 15 times till the locks hopefully are unlocked again. yeah that's dirty
	 * but no better way found yet! ;)
	 */
	if (unlikely(this_dbs_info->check_cpu_skip != 0)) {
		if (++this_dbs_info->check_cpu_skip >= 15)
		    this_dbs_info->check_cpu_skip = 0;
		return;
	}

	/*
	 * break out if we 'cannot' reduce the speed as the user might
	 * want freq_step to be zero
	 */
	if (unlikely(dbs_tuners_ins.freq_step == 0))
		return;

	/*
	 *			- Modification by ZaneZam November 2012 for ZZmoove
	 *			  Check for frequency increase is greater than hotplug up threshold value and wake up cores accordingly
	 *			  Following will bring up 3 cores in a row (cpu0 stays always on!)
	 *
	 *			- changed hotplug logic to be able to tune up threshold per core and to be able to set
	 *			  cores offline manually via sysfs
	 *
	 *			- fixed non switching cores at 0+2 and 0+3 situations
	 *			- optimized hotplug logic by removing locks and skipping hotplugging if not needed
	 *			- try to avoid deadlocks at critical events by using a flag if we are in the middle of hotplug decision
	 *
	 *			- optimised hotplug logic by reducing code and concentrating only on essential parts
	 *			- preperation for automatic core detection
	 *
	 *			- reduced hotplug loop to a minimum and use seperate functions out of dbs_check_cpu for hotplug work (credits to ktoonsez)
	 *
	 *			- added legacy mode for enabling the "old way of hotplugging" from versions 0.4/0.5
	 *                      - added hotplug idle threshold for automatic disabling of hotplugging when CPU idles
	 *                        (balanced cpu load might bring cooler cpu at that state - inspired by JustArchis observations, thx!)
	 *                      - added hotplug block cycles to reduce hotplug overhead (credits to ktoonesz)
	 *                      - added hotplug frequency thresholds (credits to Yank555)
	 *
	 *			- fixed a glitch in hotplug freq threshold tuneables
	 *
	 *              	- fixed hotplug up threshold tuneables to be able again to disable cores manually via sysfs by setting them to 0
	 *			- fixed the problem caused by a "wrong" tuneable apply order of non sticking values in hotplug down threshold tuneables
	 *			- fixed a typo in hotplug threshold tuneable macros (would have been only a issue in 8-core mode)
	 *			- fixed unwanted disabling of cores when setting hotplug threshold tuneables to lowest or highest possible value
	 *
	 */

	//  if hotplug idle threshold is reached and cpu frequency is at its minimum disable hotplug
	if (policy->cur == policy->min && max_load < dbs_tuners_ins.hotplug_idle_threshold && dbs_tuners_ins.hotplug_idle_threshold != 0 && suspend_flag == 0)
	    hotplug_idle_flag = 1;
	else
	    hotplug_idle_flag = 0;

	//  added block cycles to be able slow down hotplugging
	if ((!dbs_tuners_ins.disable_hotplug && skip_hotplug_flag == 0 && num_online_cpus() != num_possible_cpus() && policy->cur != policy->min) || hotplug_idle_flag == 1) {
	    if (hotplug_up_block_cycles > dbs_tuners_ins.hotplug_block_cycles || dbs_tuners_ins.hotplug_block_cycles == 0) {
		    schedule_work_on(0, &hotplug_online_work);
		    if (dbs_tuners_ins.hotplug_block_cycles != 0)
		    hotplug_up_block_cycles = 0;
	    }
	if (dbs_tuners_ins.hotplug_block_cycles != 0)
	hotplug_up_block_cycles++;
	}

	/* Check for frequency increase */
	if (max_load > dbs_tuners_ins.up_threshold || boost_freq) { //  Early demand - added boost switch

	    /*  Sampling down momentum - if momentum is inactive switch to "down_skip" method */
	    if (dbs_tuners_ins.sampling_down_max_mom == 0 && dbs_tuners_ins.sampling_down_factor > 1)
		this_dbs_info->down_skip = 0;

	    /* if we are already at full speed then break out early */
	    if (policy->cur == policy->max) //  changed check from reqested_freq to current freq (DerTeufel1980)
		return;

	    /*  Sampling down momentum - if momentum is active and we are switching to max speed, apply sampling_down_factor */
	    if (dbs_tuners_ins.sampling_down_max_mom != 0 && policy->cur < policy->max)
		this_dbs_info->rate_mult = dbs_tuners_ins.sampling_down_factor;

	    /*  Frequency Limit: if we are at freq_limit break out early */
	    if (dbs_tuners_ins.freq_limit != 0 && policy->cur == dbs_tuners_ins.freq_limit)
		return;

	    /*  Frequency Limit: try to strictly hold down freqency at freq_limit by spoofing requested freq */
	    if (dbs_tuners_ins.freq_limit != 0 && policy->cur > dbs_tuners_ins.freq_limit) {
		this_dbs_info->requested_freq = dbs_tuners_ins.freq_limit;

		/*  check if requested freq is higher than max freq if so bring it down to max freq (DerTeufel1980) */
		if (unlikely(this_dbs_info->requested_freq > policy->max))
		    this_dbs_info->requested_freq = policy->max;

		__cpufreq_driver_target(policy, this_dbs_info->requested_freq,
				CPUFREQ_RELATION_H);

		    /*  Sampling down momentum - calculate momentum and update sampling down factor */
		    if (dbs_tuners_ins.sampling_down_max_mom != 0 && this_dbs_info->momentum_adder < dbs_tuners_ins.sampling_down_mom_sens) {
			this_dbs_info->momentum_adder++;
			dbs_tuners_ins.sampling_down_momentum = (this_dbs_info->momentum_adder * dbs_tuners_ins.sampling_down_max_mom) / dbs_tuners_ins.sampling_down_mom_sens;
			dbs_tuners_ins.sampling_down_factor = orig_sampling_down_factor + dbs_tuners_ins.sampling_down_momentum;
		    }
		    return;

	    /*  Frequency Limit: but let it scale up as normal if the freqencies are lower freq_limit */
	    } else if (dbs_tuners_ins.freq_limit != 0 && policy->cur < dbs_tuners_ins.freq_limit) {

#ifdef ENABLE_LEGACY_MODE
			//  Legacy Mode
			if (unlikely(dbs_tuners_ins.legacy_mode == true))
				this_dbs_info->requested_freq = leg_get_next_freq(policy->cur, SCALE_FREQ_UP, max_load);
			else
#endif
				this_dbs_info->requested_freq = mn_get_next_freq(policy->cur, SCALE_FREQ_UP, max_load);

		    /*  check again if we are above limit because of fast scaling */
		    if (dbs_tuners_ins.freq_limit != 0 && this_dbs_info->requested_freq > dbs_tuners_ins.freq_limit)
			this_dbs_info->requested_freq = dbs_tuners_ins.freq_limit;

		    /*  check if requested freq is higher than max freq if so bring it down to max freq (DerTeufel1980) */
		    if (unlikely(this_dbs_info->requested_freq > policy->max))
		          this_dbs_info->requested_freq = policy->max;

		    __cpufreq_driver_target(policy, this_dbs_info->requested_freq,
				CPUFREQ_RELATION_H);

		    /*  Sampling down momentum - calculate momentum and update sampling down factor */
		    if (dbs_tuners_ins.sampling_down_max_mom != 0 && this_dbs_info->momentum_adder < dbs_tuners_ins.sampling_down_mom_sens) {
			this_dbs_info->momentum_adder++;
			dbs_tuners_ins.sampling_down_momentum = (this_dbs_info->momentum_adder * dbs_tuners_ins.sampling_down_max_mom) / dbs_tuners_ins.sampling_down_mom_sens;
			dbs_tuners_ins.sampling_down_factor = orig_sampling_down_factor + dbs_tuners_ins.sampling_down_momentum;
		    }
		return;
	    }

#ifdef ENABLE_LEGACY_MODE
	    //  Legacy Mode
	    if (unlikely(dbs_tuners_ins.legacy_mode == true))
		this_dbs_info->requested_freq = leg_get_next_freq(policy->cur, SCALE_FREQ_UP, max_load);
	     else
#endif
		this_dbs_info->requested_freq = mn_get_next_freq(policy->cur, SCALE_FREQ_UP, max_load);

	    /*  check if requested freq is higher than max freq if so bring it down to max freq (DerTeufel1980) */
	    if (unlikely(this_dbs_info->requested_freq > policy->max))
		 this_dbs_info->requested_freq = policy->max;

	    __cpufreq_driver_target(policy, this_dbs_info->requested_freq,
			CPUFREQ_RELATION_H);

	    /*  Sampling down momentum - calculate momentum and update sampling down factor */
	    if (dbs_tuners_ins.sampling_down_max_mom != 0 && this_dbs_info->momentum_adder < dbs_tuners_ins.sampling_down_mom_sens) {
		this_dbs_info->momentum_adder++;
		dbs_tuners_ins.sampling_down_momentum = (this_dbs_info->momentum_adder * dbs_tuners_ins.sampling_down_max_mom) / dbs_tuners_ins.sampling_down_mom_sens;
		dbs_tuners_ins.sampling_down_factor = orig_sampling_down_factor + dbs_tuners_ins.sampling_down_momentum;
	    }
	return;
	}

#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
	if(dbs_tuners_ins.lcdfreq_enable) {

		//  LCDFreq Scaling delays
		if( (dbs_tuners_ins.lcdfreq_kick_in_freq  <= this_dbs_info->requested_freq &&      // No core threshold, only check freq. threshold
		     dbs_tuners_ins.lcdfreq_kick_in_cores == 0                                ) ||

		    (dbs_tuners_ins.lcdfreq_kick_in_freq  <= this_dbs_info->requested_freq &&      // Core threshold reached, check freq. threshold
		     dbs_tuners_ins.lcdfreq_kick_in_cores != 0                             &&
		     dbs_tuners_ins.lcdfreq_kick_in_cores == num_online_cpus()                ) ||

		    (dbs_tuners_ins.lcdfreq_kick_in_cores != 0                             &&      // Core threshold passed, no need to check freq. threshold
		     dbs_tuners_ins.lcdfreq_kick_in_cores <  num_online_cpus()                )
		                                                                                ) {

			// We are above threshold, reset down delay, decrement up delay
			if(dbs_tuners_ins.lcdfreq_kick_in_down_left != dbs_tuners_ins.lcdfreq_kick_in_down_delay)
				dbs_tuners_ins.lcdfreq_kick_in_down_left = dbs_tuners_ins.lcdfreq_kick_in_down_delay;
			dbs_tuners_ins.lcdfreq_kick_in_up_left--;

		} else {

			// We are below threshold, reset up delay, decrement down delay
			if(dbs_tuners_ins.lcdfreq_kick_in_up_left != dbs_tuners_ins.lcdfreq_kick_in_up_delay)
				dbs_tuners_ins.lcdfreq_kick_in_up_left = dbs_tuners_ins.lcdfreq_kick_in_up_delay;
			dbs_tuners_ins.lcdfreq_kick_in_down_left--;

		}

		//  LCDFreq Scaling set frequency if needed
		if(dbs_tuners_ins.lcdfreq_kick_in_up_left <= 0 && lcdfreq_lock_current != 0) {

			// We reached up delay, set frequency to 60Hz
			lcdfreq_lock_current = 0;
			_lcdfreq_lock(lcdfreq_lock_current);

		} else if(dbs_tuners_ins.lcdfreq_kick_in_down_left <= 0 && lcdfreq_lock_current != 1) {

			// We reached down delay, set frequency to 40Hz
			lcdfreq_lock_current = 1;
			_lcdfreq_lock(lcdfreq_lock_current);

		}
	}
#endif

	/*
	 *			- Modification by ZaneZam November 2012
	 *			  Check for frequency decrease is lower than hotplug value and put cores to sleep accordingly
	 *			  Following will disable 3 cores in a row (cpu0 is always on!)
	 *
	 *			- changed logic to be able to tune down threshold per core via sysfs
	 *
	 *			- fixed non switching cores at 0+2 and 0+3 situations
	 *			- optimized hotplug logic by removing locks and skipping hotplugging if not needed
	 *			- try to avoid deadlocks at critical events by using a flag if we are in the middle of hotplug decision
	 *
	 *			- optimised hotplug logic by reducing code and concentrating only on essential parts
	 *			- preperation for automatic core detection
	 *
	 *			- reduced hotplug loop to a minimum and use seperate functions out of dbs_check_cpu for hotplug work (credits to ktoonsez)
	 *
	 *			- added legacy mode for enabling the "old way of hotplugging" from versions 0.4/0.5
	 *                      - added hotplug idle threshold for automatic disabling of hotplugging when CPU idles
	 *                        (balanced cpu load might bring cooler cpu at that state)
	 *                      - added hotplug block cycles to reduce hotplug overhead (credits to ktoonesz)
	 *                      - added hotplug frequency thresholds (credits to Yank555)
	 *
	 *			- fixed a glitch in hotplug freq threshold tuneables
	 *
	 *			- fixed hotplug up threshold tuneables to be able again to disable cores manually via sysfs by setting them to 0
	 *			- fixed the problem caused by a "wrong" tuneable apply order of non sticking values in hotplug down threshold tuneables
	 *			- fixed a typo in hotplug threshold tuneable macros (would have been only a issue in 8-core mode)
	 *			- fixed unwanted disabling of cores when setting hotplug threshold tuneables to lowest or highest possible value
	 *
	 */

	//  added block cycles to be able slow down hotplugging
	if (!dbs_tuners_ins.disable_hotplug && skip_hotplug_flag == 0 && num_online_cpus() != 1 && hotplug_idle_flag == 0) {
	    if (hotplug_down_block_cycles > dbs_tuners_ins.hotplug_block_cycles || dbs_tuners_ins.hotplug_block_cycles == 0) {
		schedule_work_on(0, &hotplug_offline_work);
		if (dbs_tuners_ins.hotplug_block_cycles != 0)
		hotplug_down_block_cycles = 0;
	    }
	if (dbs_tuners_ins.hotplug_block_cycles != 0)
	hotplug_down_block_cycles++;
	}

	/*  Sampling down momentum - if momentum is inactive switch to down skip method and if sampling_down_factor is active break out early */
	if (dbs_tuners_ins.sampling_down_max_mom == 0 && dbs_tuners_ins.sampling_down_factor > 1) {
	    if (++this_dbs_info->down_skip < dbs_tuners_ins.sampling_down_factor)
		return;
	this_dbs_info->down_skip = 0;
	}

	/*  Sampling down momentum - calculate momentum and update sampling down factor */
	if (dbs_tuners_ins.sampling_down_max_mom != 0 && this_dbs_info->momentum_adder > 1) {
	this_dbs_info->momentum_adder -= 2;
	dbs_tuners_ins.sampling_down_momentum = (this_dbs_info->momentum_adder * dbs_tuners_ins.sampling_down_max_mom) / dbs_tuners_ins.sampling_down_mom_sens;
	dbs_tuners_ins.sampling_down_factor = orig_sampling_down_factor + dbs_tuners_ins.sampling_down_momentum;
	}

	 /* Check for frequency decrease */
	if (max_load < dbs_tuners_ins.down_threshold) {

	    /*  Sampling down momentum - No longer fully busy, reset rate_mult */
	    this_dbs_info->rate_mult = 1;

		/* if we cannot reduce the frequency anymore, break out early */
		if (policy->cur == policy->min)
			return;

	/*  Frequency Limit: this should bring down freqency faster if we are coming from above limit (eg. touchboost/wakeup freqencies) */
	if (dbs_tuners_ins.freq_limit != 0 && policy->cur > dbs_tuners_ins.freq_limit) {
		this_dbs_info->requested_freq = dbs_tuners_ins.freq_limit;

#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
		if(dbs_tuners_ins.lcdfreq_enable) {

			//  LCDFreq Scaling delays
			if( (dbs_tuners_ins.lcdfreq_kick_in_freq  <= this_dbs_info->requested_freq &&      // No core threshold, only check freq. threshold
			     dbs_tuners_ins.lcdfreq_kick_in_cores == 0                                ) ||

			    (dbs_tuners_ins.lcdfreq_kick_in_freq  <= this_dbs_info->requested_freq &&      // Core threshold reached, check freq. threshold
			     dbs_tuners_ins.lcdfreq_kick_in_cores != 0                             &&
			     dbs_tuners_ins.lcdfreq_kick_in_cores == num_online_cpus()                ) ||

			    (dbs_tuners_ins.lcdfreq_kick_in_cores != 0                             &&      // Core threshold passed, no need to check freq. threshold
			     dbs_tuners_ins.lcdfreq_kick_in_cores <  num_online_cpus()                )
			                                                                                   ) {

				// We are above threshold, reset down delay, decrement up delay
				if(dbs_tuners_ins.lcdfreq_kick_in_down_left != dbs_tuners_ins.lcdfreq_kick_in_down_delay)
					dbs_tuners_ins.lcdfreq_kick_in_down_left = dbs_tuners_ins.lcdfreq_kick_in_down_delay;
				if(dbs_tuners_ins.lcdfreq_kick_in_up_left > 0)
				dbs_tuners_ins.lcdfreq_kick_in_up_left--;

			} else {

				// We are below threshold, reset up delay, decrement down delay
				if(dbs_tuners_ins.lcdfreq_kick_in_up_left != dbs_tuners_ins.lcdfreq_kick_in_up_delay)
					dbs_tuners_ins.lcdfreq_kick_in_up_left = dbs_tuners_ins.lcdfreq_kick_in_up_delay;
				if(dbs_tuners_ins.lcdfreq_kick_in_up_left > 0)
				dbs_tuners_ins.lcdfreq_kick_in_down_left--;

			}

			//  LCDFreq Scaling set frequency if needed
			if(dbs_tuners_ins.lcdfreq_kick_in_up_left <= 0 && lcdfreq_lock_current != 0) {

				// We reached up delay, set frequency to 60Hz
				lcdfreq_lock_current = 0;
				_lcdfreq_lock(lcdfreq_lock_current);

			} else if(dbs_tuners_ins.lcdfreq_kick_in_down_left <= 0 && lcdfreq_lock_current != 1) {

				// We reached down delay, set frequency to 40Hz
				lcdfreq_lock_current = 1;
				_lcdfreq_lock(lcdfreq_lock_current);

			}
		}
#endif
		__cpufreq_driver_target(policy, this_dbs_info->requested_freq,
					CPUFREQ_RELATION_L);
		return;

	/*  Frequency Limit: else we scale down as usual */
	} else if (dbs_tuners_ins.freq_limit != 0 && policy->cur <= dbs_tuners_ins.freq_limit) {

#ifdef ENABLE_LEGACY_MODE
		    //  Legacy Mode
		    if (unlikely(dbs_tuners_ins.legacy_mode == true))
			this_dbs_info->requested_freq = leg_get_next_freq(policy->cur, SCALE_FREQ_DOWN, max_load);
		    else
#endif
			this_dbs_info->requested_freq = mn_get_next_freq(policy->cur, SCALE_FREQ_DOWN, max_load);

#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
		if(dbs_tuners_ins.lcdfreq_enable) {

			//  LCDFreq Scaling delays
			if( (dbs_tuners_ins.lcdfreq_kick_in_freq  <= this_dbs_info->requested_freq &&      // No core threshold, only check freq. threshold
			     dbs_tuners_ins.lcdfreq_kick_in_cores == 0                                ) ||

			    (dbs_tuners_ins.lcdfreq_kick_in_freq  <= this_dbs_info->requested_freq &&      // Core threshold reached, check freq. threshold
			     dbs_tuners_ins.lcdfreq_kick_in_cores != 0                             &&
			     dbs_tuners_ins.lcdfreq_kick_in_cores == num_online_cpus()                ) ||

			    (dbs_tuners_ins.lcdfreq_kick_in_cores != 0                             &&      // Core threshold passed, no need to check freq. threshold
			     dbs_tuners_ins.lcdfreq_kick_in_cores <  num_online_cpus()                )
			                                                                                   ) {

				// We are above threshold, reset down delay, decrement up delay
				if(dbs_tuners_ins.lcdfreq_kick_in_down_left != dbs_tuners_ins.lcdfreq_kick_in_down_delay)
					dbs_tuners_ins.lcdfreq_kick_in_down_left = dbs_tuners_ins.lcdfreq_kick_in_down_delay;
				if(dbs_tuners_ins.lcdfreq_kick_in_up_left > 0)
				dbs_tuners_ins.lcdfreq_kick_in_up_left--;

			} else {

				// We are below threshold, reset up delay, decrement down delay
				if(dbs_tuners_ins.lcdfreq_kick_in_up_left != dbs_tuners_ins.lcdfreq_kick_in_up_delay)
					dbs_tuners_ins.lcdfreq_kick_in_up_left = dbs_tuners_ins.lcdfreq_kick_in_up_delay;
				if(dbs_tuners_ins.lcdfreq_kick_in_up_left > 0)
				dbs_tuners_ins.lcdfreq_kick_in_down_left--;

			}

			//  LCDFreq Scaling set frequency if needed
			if(dbs_tuners_ins.lcdfreq_kick_in_up_left <= 0 && lcdfreq_lock_current != 0) {

				// We reached up delay, set frequency to 60Hz
				lcdfreq_lock_current = 0;
				_lcdfreq_lock(lcdfreq_lock_current);

			} else if(dbs_tuners_ins.lcdfreq_kick_in_down_left <= 0 && lcdfreq_lock_current != 1) {

				// We reached down delay, set frequency to 40Hz
				lcdfreq_lock_current = 1;
				_lcdfreq_lock(lcdfreq_lock_current);

			}
		}
#endif
		__cpufreq_driver_target(policy, this_dbs_info->requested_freq,
					CPUFREQ_RELATION_L); //  changed to relation low
		return;
	}

#ifdef ENABLE_LEGACY_MODE
	    //  Legacy Mode
	    if (unlikely(dbs_tuners_ins.legacy_mode == true))
		this_dbs_info->requested_freq = leg_get_next_freq(policy->cur, SCALE_FREQ_DOWN, max_load);
	    else
#endif
		this_dbs_info->requested_freq = mn_get_next_freq(policy->cur, SCALE_FREQ_DOWN, max_load);

#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
		if(dbs_tuners_ins.lcdfreq_enable) {

			//  LCDFreq Scaling delays
			if( (dbs_tuners_ins.lcdfreq_kick_in_freq  <= this_dbs_info->requested_freq &&      // No core threshold, only check freq. threshold
			     dbs_tuners_ins.lcdfreq_kick_in_cores == 0                                ) ||

			    (dbs_tuners_ins.lcdfreq_kick_in_freq  <= this_dbs_info->requested_freq &&      // Core threshold reached, check freq. threshold
			     dbs_tuners_ins.lcdfreq_kick_in_cores != 0                             &&
			     dbs_tuners_ins.lcdfreq_kick_in_cores == num_online_cpus()                ) ||

			    (dbs_tuners_ins.lcdfreq_kick_in_cores != 0                             &&      // Core threshold passed, no need to check freq. threshold
			     dbs_tuners_ins.lcdfreq_kick_in_cores <  num_online_cpus()                )
			                                                                                   ) {

				// We are above threshold, reset down delay, decrement up delay
				if(dbs_tuners_ins.lcdfreq_kick_in_down_left != dbs_tuners_ins.lcdfreq_kick_in_down_delay)
					dbs_tuners_ins.lcdfreq_kick_in_down_left = dbs_tuners_ins.lcdfreq_kick_in_down_delay;
				dbs_tuners_ins.lcdfreq_kick_in_up_left--;

			} else {

				// We are below threshold, reset up delay, decrement down delay
				if(dbs_tuners_ins.lcdfreq_kick_in_up_left != dbs_tuners_ins.lcdfreq_kick_in_up_delay)
					dbs_tuners_ins.lcdfreq_kick_in_up_left = dbs_tuners_ins.lcdfreq_kick_in_up_delay;
				dbs_tuners_ins.lcdfreq_kick_in_down_left--;

			}

			//  LCDFreq Scaling set frequency if needed
			if(dbs_tuners_ins.lcdfreq_kick_in_up_left <= 0 && lcdfreq_lock_current != 0) {

				// We reached up delay, set frequency to 60Hz
				lcdfreq_lock_current = 0;
				_lcdfreq_lock(lcdfreq_lock_current);

			} else if(dbs_tuners_ins.lcdfreq_kick_in_down_left <= 0 && lcdfreq_lock_current != 1) {

				// We reached down delay, set frequency to 40Hz
				lcdfreq_lock_current = 1;
				_lcdfreq_lock(lcdfreq_lock_current);

			}
		}
#endif
		__cpufreq_driver_target(policy, this_dbs_info->requested_freq,
					CPUFREQ_RELATION_L); //  changed to relation low
		return;
	}
}

//  function for hotplug down work
static void __cpuinit hotplug_offline_work_fn(struct work_struct *work)
{
	int i=0;

#ifdef ENABLE_LEGACY_MODE
//  Legacy hotplugging
	if (unlikely(dbs_tuners_ins.legacy_mode == true && num_possible_cpus() == 4)) {
	    if (num_online_cpus() > 3) {
		if (cur_load < hotplug_thresholds[1][2] && skip_hotplug_flag == 0 && cpu_online(3))
		    cpu_down(3);
		if (cur_load < hotplug_thresholds[1][1] && skip_hotplug_flag == 0 && cpu_online(2))
		    cpu_down(2);
		if (cur_load < hotplug_thresholds[1][0] && skip_hotplug_flag == 0 && cpu_online(1))
		    cpu_down(1);
	    } else if (num_online_cpus() > 2) {
		if (cur_load < hotplug_thresholds[1][1] && skip_hotplug_flag == 0 && cpu_online(2))
		    cpu_down(2);
		if (cur_load < hotplug_thresholds[1][0] && skip_hotplug_flag == 0 && cpu_online(1))
		    cpu_down(1);
	    } else if (num_online_cpus() > 1 && cpu_online(2)) {
		if (cur_load < hotplug_thresholds[1][1] && skip_hotplug_flag == 0)
		    cpu_down(2);
	    } else if (num_online_cpus() > 1 && cpu_online(3)) {
		if (cur_load < hotplug_thresholds[1][2] && skip_hotplug_flag == 0)
		cpu_down(3);
	    } else if (num_online_cpus() > 1) {
		if (cur_load < hotplug_thresholds[1][0] && skip_hotplug_flag == 0 && cpu_online(1))
		    cpu_down(1);
	    }

	} else {
#endif
//  added frequency thresholds
	    for (i = num_possible_cpus() - 1; i >= 1; i--) {
		    if( cpu_online(i)										&&
			skip_hotplug_flag == 0									&&
			cur_load <= hotplug_thresholds[1][i-1]							&&
			(hotplug_thresholds_freq[1][i-1] == 0 || cur_freq <= hotplug_thresholds_freq[1][i-1])       )
			    cpu_down(i);
	    }
#ifdef ENABLE_LEGACY_MODE
    }
#endif
}

//  function for hotplug up work
static void __cpuinit hotplug_online_work_fn(struct work_struct *work)
{
	int i=0;

	//  enable offline cores to avoid higher / achieve balanced cpu load on idle
	if (hotplug_idle_flag == 1){
	    for (i = 1; i < num_possible_cpus(); i++) {
		if (!cpu_online(i))
		cpu_up(i);
	    }
	return;
	}

#ifdef ENABLE_LEGACY_MODE
	//  Legacy hotplugging
	if (unlikely(dbs_tuners_ins.legacy_mode == true && num_possible_cpus() == 4)) {
		if (num_online_cpus() < 2) {
		    if (hotplug_thresholds[0][0] != 0 && cur_load > hotplug_thresholds[0][0] && skip_hotplug_flag == 0 && !cpu_online(1))
			cpu_up(1);
		    if (hotplug_thresholds[0][1] != 0 && cur_load > hotplug_thresholds[0][1] && skip_hotplug_flag == 0 && !cpu_online(2))
			cpu_up(2);
		    if (hotplug_thresholds[0][2] != 0 && cur_load > hotplug_thresholds[0][2] && skip_hotplug_flag == 0 && !cpu_online(3))
			cpu_up(3);
		} else if (num_online_cpus() < 3 && cpu_online(3)) {
		    if (hotplug_thresholds[0][0] != 0 && cur_load > hotplug_thresholds[0][0] && skip_hotplug_flag == 0 && !cpu_online(1))
			cpu_up(1);
		    if (hotplug_thresholds[0][1] != 0 && cur_load > hotplug_thresholds[0][1] && skip_hotplug_flag == 0 && !cpu_online(2))
			cpu_up(2);
		} else if (num_online_cpus() < 3 && cpu_online(2)) {
		    if (hotplug_thresholds[0][0] != 0 && cur_load > hotplug_thresholds[0][0] && skip_hotplug_flag == 0 && !cpu_online(1))
			cpu_up(1);
		    if (hotplug_thresholds[0][2] != 0 && cur_load > hotplug_thresholds[0][2] && skip_hotplug_flag == 0 && !cpu_online(3))
			cpu_up(3);
		} else if (num_online_cpus() < 3) {
		    if (hotplug_thresholds[0][1] != 0 && cur_load > hotplug_thresholds[0][1] && skip_hotplug_flag == 0 && !cpu_online(2))
			cpu_up(2);
		    if (hotplug_thresholds[0][2] != 0 && cur_load > hotplug_thresholds[0][2] && skip_hotplug_flag == 0 && !cpu_online(3))
			cpu_up(3);
		} else if (num_online_cpus() < 4) {
		    if (hotplug_thresholds[0][2] != 0 && cur_load > hotplug_thresholds[0][2] && skip_hotplug_flag == 0 && !cpu_online(3))
			cpu_up(3);
		}

	} else {
#endif
	    //  added frequency thresholds
	    for (i = 1; i < num_possible_cpus(); i++) {
		    if( !cpu_online(i)										&&
			skip_hotplug_flag == 0									&&
			hotplug_thresholds[0][i-1] != 0								&&
			cur_load >= hotplug_thresholds[0][i-1]							&&
			(hotplug_thresholds_freq[0][i-1] == 0 || cur_freq >= hotplug_thresholds_freq[0][i-1])       )
			    cpu_up(i);
	    }
#ifdef ENABLE_LEGACY_MODE
	}
#endif
}

static void do_dbs_timer(struct work_struct *work)
{
	struct cpu_dbs_info_s *dbs_info =
		container_of(work, struct cpu_dbs_info_s, work.work);
	unsigned int cpu = dbs_info->cpu;

	/* We want all CPUs to do sampling nearly on same jiffy */
	int delay = usecs_to_jiffies(dbs_tuners_ins.sampling_rate * dbs_info->rate_mult); //  Sampling down momentum - added multiplier

	delay -= jiffies % delay;

	mutex_lock(&dbs_info->timer_mutex);

	dbs_check_cpu(dbs_info);

	schedule_delayed_work_on(cpu, &dbs_info->work, delay);
	mutex_unlock(&dbs_info->timer_mutex);
}

static inline void dbs_timer_init(struct cpu_dbs_info_s *dbs_info)
{
	/* We want all CPUs to do sampling nearly on same jiffy */
	int delay = usecs_to_jiffies(dbs_tuners_ins.sampling_rate);
	delay -= jiffies % delay;

	dbs_info->enable = 1;
	INIT_DELAYED_WORK_DEFERRABLE(&dbs_info->work, do_dbs_timer);
	schedule_delayed_work_on(dbs_info->cpu, &dbs_info->work, delay);
}

static inline void dbs_timer_exit(struct cpu_dbs_info_s *dbs_info)
{
	dbs_info->enable = 0;
	cancel_delayed_work(&dbs_info->work); //  Use asyncronous mode to avoid freezes / reboots when leaving impulse
}

static void powersave_early_suspend(struct early_suspend *handler)
{
  int i=0;
  struct cpufreq_frequency_table *table;				//  Use system frequency table
  skip_hotplug_flag = 1;						//  try to avoid deadlock by disabling hotplugging if we are in the middle of hotplugging logic
  suspend_flag = 1;							//  we want to know if we are at suspend because of things that shouldn't be executed at suspend
  for (i = 0; i < 1000; i++);						//  wait a few samples to be sure hotplugging is off (never be sure so this is dirty)

  table = cpufreq_frequency_get_table(0);				//  Get system frequency table

  mutex_lock(&dbs_mutex);
#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
	prev_lcdfreq_enable = dbs_tuners_ins.lcdfreq_enable;		//  LCDFreq Scaling - store state
	prev_lcdfreq_lock_current = lcdfreq_lock_current;		//  LCDFreq Scaling - store lock current
	if(dbs_tuners_ins.lcdfreq_enable) {				//  LCDFreq Scaling - reset display freq. to 60Hz only if it was enabled
		dbs_tuners_ins.lcdfreq_enable = false;
		lcdfreq_lock_current = 0;
		_lcdfreq_lock(lcdfreq_lock_current);
	}
#endif
  sampling_rate_awake = dbs_tuners_ins.sampling_rate;
  up_threshold_awake = dbs_tuners_ins.up_threshold;
  down_threshold_awake = dbs_tuners_ins.down_threshold;
  dbs_tuners_ins.sampling_down_max_mom = 0;				//  Sampling down momentum - disabled at suspend
  smooth_up_awake = dbs_tuners_ins.smooth_up;
  freq_step_awake = dbs_tuners_ins.freq_step;				//  save freq step
  freq_limit_awake = dbs_tuners_ins.freq_limit;				//  save freq limit
  fast_scaling_awake = dbs_tuners_ins.fast_scaling;			//  save scaling setting
  disable_hotplug_awake = dbs_tuners_ins.disable_hotplug;		//  save hotplug switch state

  if (dbs_tuners_ins.hotplug_sleep != 0) {				//  if set to 0 do not touch hotplugging values
	hotplug1_awake = dbs_tuners_ins.up_threshold_hotplug1;		//  save hotplug1 value for restore on awake
#if (MAX_CORES == 4 || MAX_CORES == 8)
	hotplug2_awake = dbs_tuners_ins.up_threshold_hotplug2;		//  save hotplug2 value for restore on awake
	hotplug3_awake = dbs_tuners_ins.up_threshold_hotplug3;		//  save hotplug3 value for restore on awake
#endif
#if (MAX_CORES == 8)
	hotplug4_awake = dbs_tuners_ins.up_threshold_hotplug4;		//  save hotplug4 value for restore on awake
	hotplug5_awake = dbs_tuners_ins.up_threshold_hotplug5;		//  save hotplug5 value for restore on awake
	hotplug6_awake = dbs_tuners_ins.up_threshold_hotplug6;		//  save hotplug6 value for restore on awake
	hotplug7_awake = dbs_tuners_ins.up_threshold_hotplug7;		//  save hotplug7 value for restore on awake
#endif
  }

  sampling_rate_asleep = dbs_tuners_ins.sampling_rate_sleep_multiplier; //  save sleep multiplier
  up_threshold_asleep = dbs_tuners_ins.up_threshold_sleep;		//  save up threshold
  down_threshold_asleep = dbs_tuners_ins.down_threshold_sleep;		//  save down threshold
  smooth_up_asleep = dbs_tuners_ins.smooth_up_sleep;			//  save smooth up
  freq_step_asleep = dbs_tuners_ins.freq_step_sleep;			//  save frequency step
  freq_limit_asleep = dbs_tuners_ins.freq_limit_sleep;			//  save frequency limit
  fast_scaling_asleep = dbs_tuners_ins.fast_scaling_sleep;		//  save fast scaling
  disable_hotplug_asleep = dbs_tuners_ins.disable_hotplug_sleep;	//  save disable hotplug switch
  dbs_tuners_ins.sampling_rate *= sampling_rate_asleep;			//  set sampling rate
  dbs_tuners_ins.up_threshold = up_threshold_asleep;			//  set up threshold
  dbs_tuners_ins.down_threshold = down_threshold_asleep;		//  set down threshold
  dbs_tuners_ins.smooth_up = smooth_up_asleep;				//  set smooth up
  dbs_tuners_ins.freq_step = freq_step_asleep;				//  set freqency step
  dbs_tuners_ins.freq_limit = freq_limit_asleep;			//  set freqency limit
  dbs_tuners_ins.fast_scaling = fast_scaling_asleep;			//  set fast scaling
  dbs_tuners_ins.disable_hotplug = disable_hotplug_asleep;		//  set hotplug switch

  if (dbs_tuners_ins.disable_hotplug_sleep) {				//  enable all cores at suspend if disable hotplug sleep is set
      for (i = 1; i < num_possible_cpus(); i++) {
      if (!cpu_online(i))
      cpu_up(i);
      }
  }

  if (dbs_tuners_ins.fast_scaling > 4) {				//  set scaling mode
    scaling_mode_up   = dbs_tuners_ins.fast_scaling - 4;		//  fast scaling up
    scaling_mode_down = dbs_tuners_ins.fast_scaling - 4;		//  fast scaling down
  } else {
    scaling_mode_up   = dbs_tuners_ins.fast_scaling;			//  fast scaling up only
    scaling_mode_down = 0;						//  fast scaling down
  }

  if (freq_limit_asleep == 0 ||						//  if there is no sleep freq. limit
      freq_limit_asleep > table[max_scaling_freq_hard].frequency) {	//  or it is higher than hard max freq.
	max_scaling_freq_soft = max_scaling_freq_hard;			//  use hard max freq.
	if (freq_table_order == 1)					//  if descending ordered table is used
	    limit_table_start = max_scaling_freq_soft;			//  we should use the actual scaling soft limit value as search start point
	else
	    limit_table_end = table[freq_table_size].frequency;		//  set search end point to max freq when using ascending table
  } else {
	for (i = 0; (table[i].frequency != CPUFREQ_TABLE_END); i++) {
		if (freq_limit_asleep == table[i].frequency) {		//  else lookup sleep max. frequency index
			max_scaling_freq_soft = i;
			if (freq_table_order == 1)			//  if descending ordered table is used
			    limit_table_start = max_scaling_freq_soft;	//  we should use the actual scaling soft limit value as search start point
			else
			    limit_table_end = table[i].frequency;	//  set search end point to max freq when using ascending table
			break;
		}
	}
  }

  if (dbs_tuners_ins.hotplug_sleep != 0) {				//  if set to 0 do not touch hotplugging values
	if (dbs_tuners_ins.hotplug_sleep == 1) {
	    dbs_tuners_ins.up_threshold_hotplug1 = 0;			//  set to one core
	    hotplug_thresholds[0][0] = 0;
#if (MAX_CORES == 4 || MAX_CORES == 8)
	    dbs_tuners_ins.up_threshold_hotplug2 = 0;			//  set to one core
	    hotplug_thresholds[0][1] = 0;
	    dbs_tuners_ins.up_threshold_hotplug3 = 0;			//  set to one core
	    hotplug_thresholds[0][2] = 0;
#endif
#if (MAX_CORES == 8)
	    dbs_tuners_ins.up_threshold_hotplug4 = 0;			//  set to one core
	    hotplug_thresholds[0][3] = 0;
	    dbs_tuners_ins.up_threshold_hotplug5 = 0;			//  set to one core
	    hotplug_thresholds[0][4] = 0;
	    dbs_tuners_ins.up_threshold_hotplug6 = 0;			//  set to one core
	    hotplug_thresholds[0][5] = 0;
	    dbs_tuners_ins.up_threshold_hotplug7 = 0;			//  set to one core
	    hotplug_thresholds[0][6] = 0;
#endif
	}
#if (MAX_CORES == 4 || MAX_CORES == 8)
	if (dbs_tuners_ins.hotplug_sleep == 2) {
	    dbs_tuners_ins.up_threshold_hotplug2 = 0;			//  set to two cores
	    hotplug_thresholds[0][1] = 0;
	    dbs_tuners_ins.up_threshold_hotplug3 = 0;			//  set to two cores
	    hotplug_thresholds[0][2] = 0;
#endif
#if (MAX_CORES == 8)
	    dbs_tuners_ins.up_threshold_hotplug4 = 0;			//  set to two cores
	    hotplug_thresholds[0][3] = 0;
	    dbs_tuners_ins.up_threshold_hotplug5 = 0;			//  set to two cores
	    hotplug_thresholds[0][4] = 0;
	    dbs_tuners_ins.up_threshold_hotplug6 = 0;			//  set to two cores
	    hotplug_thresholds[0][5] = 0;
	    dbs_tuners_ins.up_threshold_hotplug7 = 0;			//  set to two cores
	    hotplug_thresholds[0][6] = 0;
#endif
#if (MAX_CORES == 4 || MAX_CORES == 8)
	}
	if (dbs_tuners_ins.hotplug_sleep == 3) {
	    dbs_tuners_ins.up_threshold_hotplug3 = 0;			//  set to three cores
	    hotplug_thresholds[0][2] = 0;
#endif
#if (MAX_CORES == 8)
	    dbs_tuners_ins.up_threshold_hotplug4 = 0;			//  set to three cores
	    hotplug_thresholds[0][3] = 0;
	    dbs_tuners_ins.up_threshold_hotplug5 = 0;			//  set to three cores
	    hotplug_thresholds[0][4] = 0;
	    dbs_tuners_ins.up_threshold_hotplug6 = 0;			//  set to three cores
	    hotplug_thresholds[0][5] = 0;
	    dbs_tuners_ins.up_threshold_hotplug7 = 0;			//  set to three cores
	    hotplug_thresholds[0][6] = 0;
#endif
#if (MAX_CORES == 4 || MAX_CORES == 8)
	}
#endif
#if (MAX_CORES == 8)
	if (dbs_tuners_ins.hotplug_sleep == 4) {
	    dbs_tuners_ins.up_threshold_hotplug4 = 0;			//  set to four cores
	    hotplug_thresholds[0][3] = 0;
	    dbs_tuners_ins.up_threshold_hotplug5 = 0;			//  set to four cores
	    hotplug_thresholds[0][4] = 0;
	    dbs_tuners_ins.up_threshold_hotplug6 = 0;			//  set to four cores
	    hotplug_thresholds[0][5] = 0;
	    dbs_tuners_ins.up_threshold_hotplug7 = 0;			//  set to four cores
	    hotplug_thresholds[0][6] = 0;
	}
	if (dbs_tuners_ins.hotplug_sleep == 5) {
	    dbs_tuners_ins.up_threshold_hotplug5 = 0;			//  set to five cores
	    hotplug_thresholds[0][4] = 0;
	    dbs_tuners_ins.up_threshold_hotplug6 = 0;			//  set to five cores
	    hotplug_thresholds[0][5] = 0;
	    dbs_tuners_ins.up_threshold_hotplug7 = 0;			//  set to five cores
	    hotplug_thresholds[0][6] = 0;
	}
	if (dbs_tuners_ins.hotplug_sleep == 6) {
	    dbs_tuners_ins.up_threshold_hotplug6 = 0;			//  set to six cores
	    hotplug_thresholds[0][5] = 0;
	    dbs_tuners_ins.up_threshold_hotplug7 = 0;			//  set to six cores
	    hotplug_thresholds[0][6] = 0;
	}
	if (dbs_tuners_ins.hotplug_sleep == 7) {
	    dbs_tuners_ins.up_threshold_hotplug7 = 0;			//  set to seven cores
	    hotplug_thresholds[0][6] = 0;
	}
#endif
  }

  mutex_unlock(&dbs_mutex);
  for (i = 0; i < 1000; i++);						//  wait a few samples to be sure hotplugging is off (never be sure so this is dirty)
  skip_hotplug_flag = 0;						//  enable hotplugging again

}

static void powersave_late_resume(struct early_suspend *handler)
{
  int i=0;
  struct cpufreq_frequency_table *table;				//  Use system frequency table
  skip_hotplug_flag = 1;						//  same as above skip hotplugging to avoid deadlocks
  suspend_flag = 0;							//  we are resuming so reset supend flag

  if (!dbs_tuners_ins.disable_hotplug_sleep) {
    for (i = 1; i < num_possible_cpus(); i++) {				//  enable offline cores to avoid stuttering after resume if hotplugging limit was active
	    if (!cpu_online(i))
	    cpu_up(i);
    }
  }

  for (i = 0; i < 1000; i++);						//  wait a few samples to be sure hotplugging is off (never be sure so this is dirty)

  table = cpufreq_frequency_get_table(0);				//  Get system frequency table

 mutex_lock(&dbs_mutex);
#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
	dbs_tuners_ins.lcdfreq_enable = prev_lcdfreq_enable;		//  LCDFreq Scaling - enable it again if it was enabled
	if(dbs_tuners_ins.lcdfreq_enable) {				//  LCDFreq Scaling - restore display freq. only if it was enabled before suspend
		lcdfreq_lock_current = prev_lcdfreq_lock_current;
		_lcdfreq_lock(lcdfreq_lock_current);
	}
#endif

   if (dbs_tuners_ins.hotplug_sleep != 0) {
	dbs_tuners_ins.up_threshold_hotplug1 = hotplug1_awake;		//  restore previous settings
	hotplug_thresholds[0][0] = hotplug1_awake;
#if (MAX_CORES == 4 || MAX_CORES == 8)
	dbs_tuners_ins.up_threshold_hotplug2 = hotplug2_awake;		//  restore previous settings
	hotplug_thresholds[0][1] = hotplug2_awake;
	dbs_tuners_ins.up_threshold_hotplug3 = hotplug3_awake;		//  restore previous settings
	hotplug_thresholds[0][2] = hotplug3_awake;
#endif
#if (MAX_CORES == 8)
	dbs_tuners_ins.up_threshold_hotplug4 = hotplug4_awake;		//  restore previous settings
	hotplug_thresholds[0][3] = hotplug4_awake;
	dbs_tuners_ins.up_threshold_hotplug5 = hotplug5_awake;		//  restore previous settings
	hotplug_thresholds[0][4] = hotplug5_awake;
	dbs_tuners_ins.up_threshold_hotplug6 = hotplug6_awake;		//  restore previous settings
	hotplug_thresholds[0][5] = hotplug6_awake;
	dbs_tuners_ins.up_threshold_hotplug7 = hotplug7_awake;		//  restore previous settings
	hotplug_thresholds[0][6] = hotplug7_awake;
#endif
  }

  dbs_tuners_ins.sampling_down_max_mom = orig_sampling_down_max_mom;	//  Sampling down momentum - restore max value
  dbs_tuners_ins.sampling_rate = sampling_rate_awake;			//  restore previous settings
  dbs_tuners_ins.up_threshold = up_threshold_awake;			//  restore previous settings
  dbs_tuners_ins.down_threshold = down_threshold_awake;			//  restore previous settings
  dbs_tuners_ins.smooth_up = smooth_up_awake;				//  restore previous settings
  dbs_tuners_ins.freq_step = freq_step_awake;				//  restore previous settings
  dbs_tuners_ins.freq_limit = freq_limit_awake;				//  restore previous settings
  dbs_tuners_ins.fast_scaling = fast_scaling_awake;			//  restore previous settings
  dbs_tuners_ins.disable_hotplug = disable_hotplug_awake;		//  restore previous settings

  if (dbs_tuners_ins.fast_scaling > 4) {				//  set scaling mode
    scaling_mode_up   = dbs_tuners_ins.fast_scaling - 4;		//  fast scaling up
    scaling_mode_down = dbs_tuners_ins.fast_scaling - 4;		//  fast scaling down
  } else {
    scaling_mode_up   = dbs_tuners_ins.fast_scaling;			//  fast scaling up only
    scaling_mode_down = 0;						//  fast scaling down
  }

  if (freq_limit_awake == 0 ||						//  if there is no awake freq. limit
      freq_limit_awake > table[max_scaling_freq_hard].frequency) {	//  or it is higher than hard max freq.
	max_scaling_freq_soft = max_scaling_freq_hard;			//  use hard max freq.
	    if (freq_table_order == 1)					//  if descending ordered table is used
		limit_table_start = max_scaling_freq_soft;		//  we should use the actual scaling soft limit value as search start point
	    else
		limit_table_end = table[freq_table_size].frequency;	//  set search end point to max freq when using ascending table
  } else {
	for (i = 0; (table[i].frequency != CPUFREQ_TABLE_END); i++) {
		if (freq_limit_awake == table[i].frequency) {		//  else lookup awake max. frequency index
			max_scaling_freq_soft = i;
			if (freq_table_order == 1)			//  if descending ordered table is used
			    limit_table_start = max_scaling_freq_soft;	//  we should use the actual scaling soft limit value as search start point
			else
			    limit_table_end = table[i].frequency;	//  set search end point to soft freq limit when using ascending table
			break;
		}
	}
  }
  mutex_unlock(&dbs_mutex);
  for (i = 0; i < 1000; i++);						//  wait a few samples to be sure hotplugging is off (never be sure so this is dirty)
  skip_hotplug_flag = 0;						//  enable hotplugging again
}

static struct early_suspend _powersave_early_suspend = {
  .suspend = powersave_early_suspend,
  .resume = powersave_late_resume,
  .level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN,
};

static int cpufreq_governor_dbs(struct cpufreq_policy *policy,
				   unsigned int event)
{
	unsigned int cpu = policy->cpu;
	struct cpu_dbs_info_s *this_dbs_info;
	struct cpufreq_frequency_table *table; //  Use system frequency table
	unsigned int j;
	int rc;
	int i=0;
	int calc_index=0;

	this_dbs_info = &per_cpu(cs_cpu_dbs_info, cpu);

	table = cpufreq_frequency_get_table(0); //  Get system frequency table

	switch (event) {
	case CPUFREQ_GOV_START:
		if ((!cpu_online(cpu)) || (!policy->cur))
			return -EINVAL;

		mutex_lock(&dbs_mutex);

		for_each_cpu(j, policy->cpus) {
			struct cpu_dbs_info_s *j_dbs_info;
			j_dbs_info = &per_cpu(cs_cpu_dbs_info, j);
			j_dbs_info->cur_policy = policy;

			j_dbs_info->prev_cpu_idle = get_cpu_idle_time(j,
						&j_dbs_info->prev_cpu_wall);
			if (dbs_tuners_ins.ignore_nice) {
				j_dbs_info->prev_cpu_nice =
						kstat_cpu(j).cpustat.nice;
			}
			j_dbs_info->time_in_idle = get_cpu_idle_time_us(cpu, &j_dbs_info->idle_exit_time); //  added idle exit time handling
		}
		this_dbs_info->cpu = cpu;		//  Initialise the cpu field during conservative governor start (https://github.com/ktoonsez/KT747-JB/commit/298dd04a610a6a655d7b77f320198d9f6c7565b6)
		this_dbs_info->rate_mult = 1;		//  Sampling down momentum - reset multiplier
		this_dbs_info->momentum_adder = 0;	//  Sampling down momentum - reset momentum adder
		this_dbs_info->down_skip = 0;		//  Sampling down - reset down_skip
		this_dbs_info->check_cpu_skip = 1;	//  we do not want to crash because of hotplugging so we start without it by skipping check_cpu
		this_dbs_info->requested_freq = policy->cur;

		//  save default values in threshold array
		for (i = 0; i < num_possible_cpus(); i++) {
		    hotplug_thresholds[0][i] = DEF_FREQUENCY_UP_THRESHOLD_HOTPLUG;
		    hotplug_thresholds[1][i] = DEF_FREQUENCY_DOWN_THRESHOLD_HOTPLUG;
		    hotplug_thresholds_tuneable[i] = 1;
		}

		//  initialisation of freq search in scaling table
		for (i = 0; (table[i].frequency != CPUFREQ_TABLE_END); i++) {
			if (policy->max == table[i].frequency) {
				max_scaling_freq_hard = max_scaling_freq_soft = i; //  init soft and hard value
				//  Continue looping until table end is reached, we need this to set the table size limit below
			}
		}

		freq_table_size = i - 1; //  upper index limit of freq. table

		/*
		 *  we have to take care about where we are in the frequency table. when using kernel sources without OC capability
		 * it might be that index 0 and 1 contains no frequencies so a save index start point is needed.
		 */
		calc_index = freq_table_size - max_scaling_freq_hard;	//  calculate the difference and use it as start point
		if (calc_index == freq_table_size)			//  if we are at the end of the table
		    calc_index = calc_index - 1;			//  shift in range for order calculation below

		//  assert if CPU freq. table is in ascending or descending order
		if (table[calc_index].frequency > table[calc_index+1].frequency) {
			freq_table_order = +1;				//  table is in descending order as expected, lowest freq at the bottom of the table
			min_scaling_freq = i - 1;			//  last valid frequency step (lowest frequency)
			limit_table_start = max_scaling_freq_soft;	//  we should use the actual scaling soft limit value as search start point
		} else {
			freq_table_order = -1;				//  table is in ascending order, lowest freq at the top of the table
			min_scaling_freq = 0;				//  first valid frequency step (lowest frequency)
			limit_table_start = 0;				//  start searching at lowest freq
			limit_table_end = table[freq_table_size].frequency; //  end searching at highest freq limit
		}

		mutex_init(&this_dbs_info->timer_mutex);
		dbs_enable++;

		/*
		 * Start the timerschedule work, when this governor
		 * is used for first time
		 */
		if (dbs_enable == 1) {
			unsigned int latency;
			/* policy latency is in nS. Convert it to uS first */
			latency = policy->cpuinfo.transition_latency / 1000;
			if (latency == 0)
				latency = 1;

			rc = sysfs_create_group(cpufreq_global_kobject,
						&dbs_attr_group);
			if (rc) {
				mutex_unlock(&dbs_mutex);
				return rc;
			}

			/*
			 * conservative does not implement micro like ondemand
			 * governor, thus we are bound to jiffes/HZ
			 */
			min_sampling_rate =
				MIN_SAMPLING_RATE_RATIO * jiffies_to_usecs(3);
			/* Bring kernel and HW constraints together */
			min_sampling_rate = max(min_sampling_rate,
					MIN_LATENCY_MULTIPLIER * latency);
			dbs_tuners_ins.sampling_rate =
				max(min_sampling_rate,
				    latency * LATENCY_MULTIPLIER);
			orig_sampling_down_factor = dbs_tuners_ins.sampling_down_factor;	//  Sampling down momentum - set down factor
			orig_sampling_down_max_mom = dbs_tuners_ins.sampling_down_max_mom;	//  Sampling down momentum - set max momentum
			sampling_rate_awake = dbs_tuners_ins.sampling_rate;
			up_threshold_awake = dbs_tuners_ins.up_threshold;
			down_threshold_awake = dbs_tuners_ins.down_threshold;
			smooth_up_awake = dbs_tuners_ins.smooth_up;
			cpufreq_register_notifier(
					&dbs_cpufreq_notifier_block,
					CPUFREQ_TRANSITION_NOTIFIER);
		}
		mutex_unlock(&dbs_mutex);
		dbs_timer_init(this_dbs_info);
	        register_early_suspend(&_powersave_early_suspend);
		break;

	case CPUFREQ_GOV_STOP:
		skip_hotplug_flag = 1;			//  disable hotplugging during stop to avoid deadlocks if we are in the hotplugging logic
		this_dbs_info->check_cpu_skip = 1;	//  and we disable cpu_check also on next 15 samples

		mutex_lock(&dbs_mutex);			//  added for deadlock fix on governor stop
		dbs_timer_exit(this_dbs_info);
		mutex_unlock(&dbs_mutex);		//  added for deadlock fix on governor stop

		this_dbs_info->idle_exit_time = 0;	//  added idle exit time handling

		mutex_lock(&dbs_mutex);
		dbs_enable--;
		mutex_destroy(&this_dbs_info->timer_mutex);
		/*
		 * Stop the timerschedule work, when this governor
		 * is used for first time
		 */
		if (dbs_enable == 0)
			cpufreq_unregister_notifier(
					&dbs_cpufreq_notifier_block,
					CPUFREQ_TRANSITION_NOTIFIER);

		mutex_unlock(&dbs_mutex);
		if (!dbs_enable)
			sysfs_remove_group(cpufreq_global_kobject,
					   &dbs_attr_group);

	        unregister_early_suspend(&_powersave_early_suspend);

#ifdef CONFIG_CPU_FREQ_LCD_FREQ_DFS
		if (dbs_tuners_ins.lcdfreq_enable == true) {
			lcdfreq_lock_current = 0;		//  LCDFreq Scaling disable at stop
			_lcdfreq_lock(lcdfreq_lock_current);	//  LCDFreq Scaling disable at stop
		}
#endif
		break;

	case CPUFREQ_GOV_LIMITS:
		skip_hotplug_flag = 1;			//  disable hotplugging during limit change
		this_dbs_info->check_cpu_skip = 1;	//  to avoid deadlocks skip check_cpu next 25 samples
		for (i = 0; i < 1000; i++);		//  wait a few samples to be sure hotplugging is off (never be sure so this is dirty)
		/*
		 *  we really want to do this limit update but here are deadlocks possible if hotplugging locks are active, so if we are about
		 * to crash skip the whole freq limit change attempt by using mutex_trylock instead of mutex_lock.
		 * so now this is a real fix but on the other hand it could also avoid limit changes so we keep all the other workarounds
		 * to reduce the chance of such situations!
		 */
		if (mutex_trylock(&this_dbs_info->timer_mutex)) {
		    if (policy->max < this_dbs_info->cur_policy->cur)
			__cpufreq_driver_target(
					this_dbs_info->cur_policy,
					policy->max, CPUFREQ_RELATION_H);
		    else if (policy->min > this_dbs_info->cur_policy->cur)
			__cpufreq_driver_target(
					this_dbs_info->cur_policy,
					policy->min, CPUFREQ_RELATION_L);
		    mutex_unlock(&this_dbs_info->timer_mutex);
		} else {
		    return 0;
		}
		/*
		*  obviously this "limit case" will be executed multiple times at suspend (not sure why!?)
		* but we have already a early suspend code to handle scaling search limits so we have to use a flag to avoid double execution at suspend!
		*/

		if (unlikely(suspend_flag == 0 && policy->max != table[max_scaling_freq_hard].frequency)) {		//  if policy->max has changed and we are not sleeping
		    for (i = 0; (table[i].frequency != CPUFREQ_TABLE_END); i++) {
			if (policy->max == table[i].frequency) {
				max_scaling_freq_hard = i;								// ZZ   : set new freq scaling number
				break;
			}
		    }

		    if (unlikely(table[max_scaling_freq_soft].frequency > table[max_scaling_freq_hard].frequency)) {	//  if we would go above hard limits reset them
			    max_scaling_freq_soft = max_scaling_freq_hard;						//  if soft freq. is higher than hard max limit then set it to hard max limit value
				if (freq_table_order == 1)								//  if descending ordered table is used
				    limit_table_start = max_scaling_freq_soft;						//  we should use the actual scaling soft limit value as search starting point
				else
				    limit_table_end = policy->max;							//  prepare max search range for ascending ordered table
			    if (policy->max <= dbs_tuners_ins.freq_limit)						// ZZ   : check limit
				dbs_tuners_ins.freq_limit = 0;								//  and delete active limit if it is above hard limit
			    if (policy->max <= dbs_tuners_ins.freq_limit_sleep)						// ZZ   : check sleep limit
				dbs_tuners_ins.freq_limit_sleep = 0;							//  if we would go also above this hard limit delete soft limit also
		    } else if (unlikely(table[max_scaling_freq_soft].frequency < table[max_scaling_freq_hard].frequency
										 && dbs_tuners_ins.freq_limit == 0)) {
			    max_scaling_freq_soft = max_scaling_freq_hard;						//  if no limit is set and soft freq lower than limit then set back to hard max limit value
				if (freq_table_order == 1)								//  if descending ordered table is used
				    limit_table_start = max_scaling_freq_soft;						//  we should use the actual scaling soft limit value as search starting point
				else
				    limit_table_end = policy->max;							//  prepare max search range for ascending ordered table
		    }
		}

		skip_hotplug_flag = 0;											//  enable hotplugging again
		this_dbs_info->time_in_idle = get_cpu_idle_time_us(cpu, &this_dbs_info->idle_exit_time);		//  added idle exit time handling
		break;
	}
	return 0;
}

#ifndef CONFIG_CPU_FREQ_DEFAULT_GOV_IMPULSE
static
#endif
struct cpufreq_governor cpufreq_gov_impulse = {
	.name			= "impulse",
	.governor		= cpufreq_governor_dbs,
	.max_transition_latency	= TRANSITION_LATENCY_LIMIT,
	.owner			= THIS_MODULE,
};

static int __init cpufreq_gov_dbs_init(void) //  added idle exit time handling
{
    unsigned int i;
    struct cpu_dbs_info_s *this_dbs_info;
    /* Initalize per-cpu data: */
    for_each_possible_cpu(i) {
	this_dbs_info = &per_cpu(cs_cpu_dbs_info, i);
	this_dbs_info->time_in_idle = 0;
	this_dbs_info->idle_exit_time = 0;
    }

    INIT_WORK(&hotplug_offline_work, hotplug_offline_work_fn); //  init hotplug work
    INIT_WORK(&hotplug_online_work, hotplug_online_work_fn); //  init hotplug work

	return cpufreq_register_governor(&cpufreq_gov_impulse);
}

static void __exit cpufreq_gov_dbs_exit(void)
{
	cpufreq_unregister_governor(&cpufreq_gov_impulse);
}

/*
 * Impulse governor is based on "zzmoove 0.4 developed by Zane Zaminsky <cyxman@yahoo.com>
 * Source Code zzmoove 0.7d: https://github.com/zanezam/cpufreq-governor-zzmoove/commit/31ad5b71fde359e46fe4924b8b852de56f42e2ed "
 *
 * and zzmoove governor is based on the modified conservative (original author
 * Alexander Clouter <alex@digriz.org.uk>) smoove governor from Michael
 * Weingaertner <mialwe@googlemail.com> (source: https://github.com/mialwe/mngb/)
 * Modified by Zane Zaminsky November 2012 to be hotplug-able and optimzed for use
 * with Samsung I9300. CPU Hotplug modifications partially taken from ktoonservative
 * governor from ktoonsez KT747-JB kernel (https://github.com/ktoonsez/KT747-JB)
 */

MODULE_AUTHOR("Javier Sayago <admin@lonasdigital.com>");
MODULE_DESCRIPTION("'cpufreq_impulse' - A dynamic cpufreq/cpuhotplug impulse governor");
MODULE_LICENSE("GPL");

#ifdef CONFIG_CPU_FREQ_DEFAULT_GOV_IMPULSE
fs_initcall(cpufreq_gov_dbs_init);
#else
module_init(cpufreq_gov_dbs_init);
#endif
module_exit(cpufreq_gov_dbs_exit);
