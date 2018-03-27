/*
 * Copyright (C) 2018 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __POWER_HELPER_H__
#define __POWER_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hardware/hardware.h>
#include <hardware/power.h>

#define BOOSTPULSE_PATH "/sys/devices/system/cpu/cpufreq/interactive/boostpulse"
#define CPU_MAX_FREQ_PATH "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
#define FACEDOWN_PATH "/sys/class/htc_sensorhub/sensor_hub/facedown_enabled"
#define TOUCH_SYNA_INTERACTIVE_PATH "/sys/devices/platform/spi-tegra114.2/spi_master/spi2/spi2.0/input/input0/interactive"
#define WAKE_GESTURE_PATH "/sys/devices/platform/spi-tegra114.2/spi_master/spi2/spi2.0/input/input0/wake_gesture"
#define GPU_BOOST_PATH "/dev/constraint_gpu_freq"
#define IO_IS_BUSY_PATH "/sys/devices/system/cpu/cpufreq/interactive/io_is_busy"

#define LOW_POWER_MAX_FREQ "1020000"
#define NORMAL_MAX_FREQ "2901000"
#define GPU_FREQ_CONSTRAINT "852000 852000 -1 2000"

#define SVELTE_PROP "ro.boot.svelte"
#define SVELTE_MAX_FREQ_PROP "ro.config.svelte.max_cpu_freq"
#define SVELTE_LOW_POWER_MAX_FREQ_PROP "ro.config.svelte.low_power_max_cpu_freq"

static void sysfs_write(const char *path, char *s);
static void calculate_max_cpu_freq();
static int boostpulse_open();

void power_init(void);
void power_set_interactive(int on);
void set_feature(feature_t feature, int state);
void power_hint(power_hint_t hint, void *data);

#ifdef __cplusplus
}
#endif

#endif //__POWER_HELPER_H__
