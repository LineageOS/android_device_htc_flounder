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

#include <dumpstate.h>

#ifdef HAS_DENVER_UID_CHECK
extern int denver_uid_check(void);
#endif

void dumpstate_board()
{
    Dumpstate& ds = Dumpstate::GetInstance();

#ifdef HAS_DENVER_UID_CHECK
    denver_uid_check();
#endif

    ds.DumpFile("soc revision", "/sys/devices/soc0/revision");
    ds.DumpFile("soc die_id", "/sys/devices/soc0/soc_id");
    ds.DumpFile("bq2419x charger regs", "/d/bq2419x-regs");
    ds.DumpFile("max17050 fuel gauge regs", "/d/max17050-regs");
    ds.DumpFile("shrinkers", "/d/shrinker");
    ds.DumpFile("wlan", "/sys/module/bcmdhd/parameters/info_string");
    ds.DumpFile("display controller", "/d/tegradc.0/stats");
    ds.DumpFile("sensor_hub version", "/sys/devices/virtual/htc_sensorhub/sensor_hub/firmware_version");
    ds.DumpFile("audio nvavp", "/d/nvavp/refs");
};
