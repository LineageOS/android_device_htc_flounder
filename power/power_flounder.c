/*
 * Copyright (C) 2013 The Android Open Source Project
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

#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/time.h>
#include <stdbool.h>
//#define LOG_NDEBUG 0

#define LOG_TAG "FlounderPowerHAL"
#include <utils/Log.h>

#include <hardware/hardware.h>
#include <hardware/power.h>

#define BOOSTPULSE_PATH "/sys/devices/system/cpu/cpufreq/interactive/boostpulse"
#define BOOST_PATH "/sys/devices/system/cpu/cpufreq/interactive/boost"
#define CPU_MAX_FREQ_PATH "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
#define FACEDOWN_PATH "/sys/class/htc_sensorhub/sensor_hub/facedown_enabled"
//BOOST_PULSE_DURATION and BOOT_PULSE_DURATION_STR should always be in sync
#define BOOST_PULSE_DURATION 1000000
#define BOOST_PULSE_DURATION_STR "1000000"
#define NSEC_PER_SEC 1000000000
#define USEC_PER_SEC 1000000
#define NSEC_PER_USEC 100
#define LOW_POWER_MAX_FREQ "1020000"
#define NORMAL_MAX_FREQ "2901000"

struct flounder_power_module {
    struct power_module base;
    pthread_mutex_t lock;
    int boostpulse_fd;
    int boostpulse_warned;
};

static unsigned int vsync_count;
static struct timespec last_touch_boost;
static bool touch_boost;
static bool low_power_mode = false;

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

static void power_init(struct power_module *module)
{
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/timer_rate",
                "20000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/timer_slack",
                "20000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/min_sample_time",
                "80000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/hispeed_freq",
                "624000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/go_hispeed_load",
                "99");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/target_loads", "65 228000:75 624000:85");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/above_hispeed_delay",
                "20000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/boostpulse_duration",
                BOOST_PULSE_DURATION_STR);
    sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/io_is_busy", "0");
}

static void power_set_interactive(struct power_module *module, int on)
{
    ALOGV("power_set_interactive: %d\n", on);

    /*
     * Lower maximum frequency when screen is off.
     */
    sysfs_write(CPU_MAX_FREQ_PATH,
                (!on || low_power_mode) ? LOW_POWER_MAX_FREQ : NORMAL_MAX_FREQ);
    sysfs_write(FACEDOWN_PATH, on ? "0" : "1");
    ALOGV("power_set_interactive: %d done\n", on);
}

static int boostpulse_open(struct flounder_power_module *flounder)
{
    char buf[80];

    pthread_mutex_lock(&flounder->lock);

    if (flounder->boostpulse_fd < 0) {
        flounder->boostpulse_fd = open(BOOSTPULSE_PATH, O_WRONLY);

        if (flounder->boostpulse_fd < 0) {
            if (!flounder->boostpulse_warned) {
                strerror_r(errno, buf, sizeof(buf));
                ALOGE("Error opening %s: %s\n", BOOSTPULSE_PATH, buf);
                flounder->boostpulse_warned = 1;
            }
        }
    }

    pthread_mutex_unlock(&flounder->lock);
    return flounder->boostpulse_fd;
}

static struct timespec timespec_diff(struct timespec lhs, struct timespec rhs)
{
    struct timespec result;
    if (rhs.tv_nsec > lhs.tv_nsec) {
        result.tv_sec = lhs.tv_sec - rhs.tv_sec - 1;
        result.tv_nsec = NSEC_PER_SEC + lhs.tv_nsec - rhs.tv_nsec;
    } else {
        result.tv_sec = lhs.tv_sec - rhs.tv_sec;
        result.tv_nsec = lhs.tv_nsec - rhs.tv_nsec;
    }
    return result;
}

static int check_boostpulse_on(struct timespec diff)
{
    long boost_ns = (BOOST_PULSE_DURATION * NSEC_PER_USEC) % NSEC_PER_SEC;
    long boost_s = BOOST_PULSE_DURATION / USEC_PER_SEC;

    if (diff.tv_sec == boost_s)
        return (diff.tv_nsec < boost_ns);
    return (diff.tv_sec < boost_s);
}

static void flounder_power_hint(struct power_module *module, power_hint_t hint,
                                void *data)
{
    struct flounder_power_module *flounder =
            (struct flounder_power_module *) module;
    struct timespec now, diff;
    char buf[80];
    int len;

    switch (hint) {
     case POWER_HINT_INTERACTION:
        if (boostpulse_open(flounder) >= 0) {
            pthread_mutex_lock(&flounder->lock);
            len = write(flounder->boostpulse_fd, "1", 1);

            if (len < 0) {
                strerror_r(errno, buf, sizeof(buf));
                ALOGE("Error writing to %s: %s\n", BOOSTPULSE_PATH, buf);
            } else {
                clock_gettime(CLOCK_MONOTONIC, &last_touch_boost);
                touch_boost = true;
            }
            pthread_mutex_unlock(&flounder->lock);
        }

        break;

   case POWER_HINT_VSYNC:
        pthread_mutex_lock(&flounder->lock);
        if (data) {
            if (vsync_count < UINT_MAX)
                vsync_count++;
        } else {
            if (vsync_count)
                vsync_count--;
            if (vsync_count == 0 && touch_boost) {
                touch_boost = false;
                clock_gettime(CLOCK_MONOTONIC, &now);
                diff = timespec_diff(now, last_touch_boost);
                if (check_boostpulse_on(diff)) {
                    sysfs_write(BOOST_PATH, "0");
                }
            }
        }
        pthread_mutex_unlock(&flounder->lock);
        break;

    case POWER_HINT_LOW_POWER:
        pthread_mutex_lock(&flounder->lock);
        if (data) {
            sysfs_write(CPU_MAX_FREQ_PATH, LOW_POWER_MAX_FREQ);
            // reduces the refresh rate
            system("service call SurfaceFlinger 1016 i32 1");
        } else {
            sysfs_write(CPU_MAX_FREQ_PATH, NORMAL_MAX_FREQ);
            // restores the refresh rate
            system("service call SurfaceFlinger 1016 i32 0");
        }
        low_power_mode = data;
        pthread_mutex_unlock(&flounder->lock);
        break;

    default:
            break;
    }
}

static struct hw_module_methods_t power_module_methods = {
    .open = NULL,
};

struct flounder_power_module HAL_MODULE_INFO_SYM = {
    base: {
        common: {
            tag: HARDWARE_MODULE_TAG,
            module_api_version: POWER_MODULE_API_VERSION_0_2,
            hal_api_version: HARDWARE_HAL_API_VERSION,
            id: POWER_HARDWARE_MODULE_ID,
            name: "Flounder Power HAL",
            author: "The Android Open Source Project",
            methods: &power_module_methods,
        },

        init: power_init,
        setInteractive: power_set_interactive,
        powerHint: flounder_power_hint,
    },

    lock: PTHREAD_MUTEX_INITIALIZER,
    boostpulse_fd: -1,
    boostpulse_warned: 0,
};

