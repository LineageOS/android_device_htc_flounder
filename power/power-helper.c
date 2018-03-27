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

#define LOG_TAG "power-helper"

#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cutils/properties.h>
#include <utils/Log.h>

#include "power-helper.h"

static int boostpulse_fd;
static bool boostpulse_warned = false;

static bool low_power_mode = false;

static char *max_cpu_freq = NORMAL_MAX_FREQ;
static char *low_power_max_cpu_freq = LOW_POWER_MAX_FREQ;

static void sysfs_write(const char *path, char *s)
{
    char buf[80];
    int len;
    int fd = open(path, O_WRONLY);

    if (fd < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error opening %s: %s\n", path, buf);
        return;
    }

    len = write(fd, s, strlen(s));
    if (len < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error writing to %s: %s\n", path, buf);
    }

    close(fd);
}

static void calculate_max_cpu_freq() {
    int32_t is_svelte = property_get_int32(SVELTE_PROP, 0);

    if (is_svelte) {
        char prop_buffer[PROPERTY_VALUE_MAX];
        int len = property_get(SVELTE_MAX_FREQ_PROP, prop_buffer, LOW_POWER_MAX_FREQ);
        max_cpu_freq = strndup(prop_buffer, len);
        len = property_get(SVELTE_LOW_POWER_MAX_FREQ_PROP, prop_buffer, LOW_POWER_MAX_FREQ);
        low_power_max_cpu_freq = strndup(prop_buffer, len);
    }
}

void power_init(void)
{
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/timer_rate",
                "20000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/timer_slack",
                "20000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/min_sample_time",
                "80000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/hispeed_freq",
                "1530000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/go_hispeed_load",
                "99");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/target_loads",
                "65 228000:75 624000:85");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/above_hispeed_delay",
                "20000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/boostpulse_duration",
                "1000000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/io_is_busy", "0");

    calculate_max_cpu_freq();
}

void power_set_interactive(int on)
{
    ALOGV("power_set_interactive: %d\n", on);

    /*
     * Lower maximum frequency when screen is off.
     */
    sysfs_write(CPU_MAX_FREQ_PATH,
                (!on || low_power_mode) ? low_power_max_cpu_freq : max_cpu_freq);
    sysfs_write(IO_IS_BUSY_PATH, on ? "1" : "0");
    sysfs_write(FACEDOWN_PATH, on ? "0" : "1");
    sysfs_write(TOUCH_SYNA_INTERACTIVE_PATH, on ? "1" : "0");
    ALOGV("power_set_interactive: %d done\n", on);
}

static int boostpulse_open()
{
    char buf[80];
    int len;
    static int gpu_boost_fd = -1;

    if (boostpulse_fd < 0) {
        boostpulse_fd = open(BOOSTPULSE_PATH, O_WRONLY);

        if (boostpulse_fd < 0) {
            if (!boostpulse_warned) {
                strerror_r(errno, buf, sizeof(buf));
                ALOGE("Error opening %s: %s\n", BOOSTPULSE_PATH, buf);
                boostpulse_warned = true;
            }
        }
    }
    {
        if (gpu_boost_fd == -1)
            gpu_boost_fd = open(GPU_BOOST_PATH, O_WRONLY);

        if (gpu_boost_fd < 0) {
            strerror_r(errno, buf, sizeof(buf));
            ALOGE("Error opening %s: %s\n", GPU_BOOST_PATH, buf);
        } else {
            len = write(gpu_boost_fd, GPU_FREQ_CONSTRAINT,
                        strlen(GPU_FREQ_CONSTRAINT));
            if (len < 0) {
                strerror_r(errno, buf, sizeof(buf));
                ALOGE("Error writing to %s: %s\n", GPU_BOOST_PATH, buf);
            }
        }
    }

    return boostpulse_fd;
}

void set_feature(feature_t feature, int state)
{
    switch (feature) {
    case POWER_FEATURE_DOUBLE_TAP_TO_WAKE:
        sysfs_write(WAKE_GESTURE_PATH, state ? "1" : "0");
        break;
    default:
        ALOGW("Error setting the feature, it doesn't exist %d\n", feature);
        break;
    }
}

void power_hint(power_hint_t hint, void *data)
{
    char buf[80];
    int len;

    switch (hint) {
    case POWER_HINT_INTERACTION:
        if (boostpulse_open() >= 0) {
            len = write(boostpulse_fd, "1", 1);

            if (len < 0) {
                strerror_r(errno, buf, sizeof(buf));
                ALOGE("Error writing to %s: %s\n", BOOSTPULSE_PATH, buf);
            }
        }
        break;
    case POWER_HINT_VSYNC:
        break;
    case POWER_HINT_LOW_POWER:
        if (data) {
            sysfs_write(CPU_MAX_FREQ_PATH, low_power_max_cpu_freq);
            low_power_mode = true;
        } else {
            sysfs_write(CPU_MAX_FREQ_PATH, max_cpu_freq);
            low_power_mode = false;
        }
        break;
    default:
        break;
    }
}
