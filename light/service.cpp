/*
 * Copyright 2018 The LineageOS Project
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

#define LOG_TAG "android.hardware.light@2.0-service.flounder"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>
#include <utils/Errors.h>

#include "Light.h"

using android::sp;
using android::status_t;
using android::OK;

// libhwbinder:
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

// Generated HIDL files
using android::hardware::light::V2_0::ILight;
using android::hardware::light::V2_0::implementation::Light;

const static std::string kBacklightPath = "/sys/class/backlight/tegra-dsi-backlight.0/brightness";
const static std::string kLedPath = "/sys/class/htc_sensorhub/sensor_hub/led_en";

int main() {
    status_t status;
    android::sp<ILight> service = nullptr;

    LOG(INFO) << "Light HAL Service 2.0 is starting.";

    std::ofstream backlight(kBacklightPath);
    if (!backlight) {
        int error = errno;
        LOG(ERROR) << "Failed to open " << kBacklightPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -error;
    }

    std::ofstream led(kLedPath);
    if (!led) {
        int error = errno;
        LOG(ERROR) << "Failed to open " << kLedPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -error;
    }

    service = new Light(std::move(backlight), std::move(led));
    if (service == nullptr) {
        LOG(ERROR) << "Can not create an instance of Light HAL Iface, exiting.";
        goto shutdown;
    }

    configureRpcThreadpool(1, true);

    status = service->registerAsService();
    if (status != OK) {
        LOG(ERROR) << "Could not register service for Light HAL Iface (" << status << ")";
        goto shutdown;
    }

    LOG(INFO) << "Light HAL Service is ready.";
    joinRpcThreadpool();

shutdown:
    // Under normal cases, execution will not reach this line.
    LOG(ERROR) << "Light HAL Service is shutting down.";
    return 1;
}
