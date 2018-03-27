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

#include <hardware/power.h>

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
