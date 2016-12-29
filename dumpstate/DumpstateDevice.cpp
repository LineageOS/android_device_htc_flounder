/*
 * Copyright (C) 2016 The Android Open Source Project
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

#define LOG_TAG "dumpstate"

#include "DumpstateDevice.h"

#include <log/log.h>

#include "DumpstateUtil.h"

#ifdef HAS_DENVER_UID_CHECK
extern int denver_uid_check(void);
#endif

using android::os::dumpstate::CommandOptions;
using android::os::dumpstate::DumpFileToFd;
using android::os::dumpstate::RunCommandToFd;

namespace android {
namespace hardware {
namespace dumpstate {
namespace V1_0 {
namespace implementation {

// Methods from ::android::hardware::dumpstate::V1_0::IDumpstateDevice follow.
Return<void> DumpstateDevice::dumpstateBoard(const hidl_handle& handle) {
    if (handle->numFds < 1) {
        ALOGE("no FDs\n");
        return Void();
    }

    int fd = handle->data[0];
    if (fd < 0) {
        ALOGE("invalid FD: %d\n", handle->data[0]);
        return Void();
    }

#ifdef HAS_DENVER_UID_CHECK
    denver_uid_check();
#endif

    DumpFileToFd(fd, "soc revision", "/sys/devices/soc0/revision");
    DumpFileToFd(fd, "soc die_id", "/sys/devices/soc0/soc_id");
    DumpFileToFd(fd, "bq2419x charger regs", "/d/bq2419x-regs");
    DumpFileToFd(fd, "max17050 fuel gauge regs", "/d/max17050-regs");
    DumpFileToFd(fd, "shrinkers", "/d/shrinker");
    DumpFileToFd(fd, "wlan", "/sys/module/bcmdhd/parameters/info_string");
    DumpFileToFd(fd, "display controller", "/d/tegradc.0/stats");
    DumpFileToFd(fd, "sensor_hub version", "/sys/devices/virtual/htc_sensorhub/sensor_hub/firmware_version");
    DumpFileToFd(fd, "audio nvavp", "/d/nvavp/refs");

    return Void();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace dumpstate
}  // namespace hardware
}  // namespace android
