/*
 * Copyright (C) 2014 The Android Open Source Project
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

#define LOG_TAG "healthd-flounder"
#include <healthd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cutils/klog.h>
#include <sys/types.h>

/* Nominal voltage for ENERGY_COUNTER computation */
#define VOLTAGE_NOMINAL 3.7

#define POWER_SUPPLY_SUBSYSTEM "power_supply"
#define POWER_SUPPLY_SYSFS_PATH "/sys/class/" POWER_SUPPLY_SUBSYSTEM

#define MAX17050_PATH POWER_SUPPLY_SYSFS_PATH "/battery"
#define CHARGE_COUNTER_EXT_PATH MAX17050_PATH "/charge_counter_ext"

using namespace android;

int healthd_board_battery_update(struct BatteryProperties *props)
{
    // return 0 to log periodic polled battery status to kernel log
    return 0;
}

static int read_sysfs(const char *path, char *buf, size_t size) {
    char *cp = NULL;

    int fd = open(path, O_RDONLY, 0);
    if (fd == -1) {
        KLOG_ERROR(LOG_TAG, "Could not open '%s'\n", path);
        return -1;
    }

    ssize_t count = TEMP_FAILURE_RETRY(read(fd, buf, size));
    if (count > 0)
        cp = (char *)memrchr(buf, '\n', count);

    if (cp)
        *cp = '\0';
    else
        buf[0] = '\0';

    close(fd);
    return count;
}

static int64_t get_int64_field(const char *path) {
    const int SIZE = 21;
    char buf[SIZE];

    int64_t value = 0;
    if (read_sysfs(path, buf, SIZE) > 0) {
        value = strtoll(buf, NULL, 0);
    }
    return value;
}

static int flounder_energy_counter(int64_t *energy)
{
    *energy = get_int64_field(CHARGE_COUNTER_EXT_PATH) * VOLTAGE_NOMINAL;
    return 0;
}

void healthd_board_init(struct healthd_config *config)
{
    config->energyCounter = flounder_energy_counter;
}
