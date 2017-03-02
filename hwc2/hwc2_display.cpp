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

#include <cutils/log.h>
#include <inttypes.h>

#include <vector>
#include <array>

#include "hwc2.h"

uint64_t hwc2_display::display_cnt = 0;

hwc2_display::hwc2_display(hwc2_display_t id, int adf_intf_fd,
        const struct adf_device &adf_dev, hwc2_connection_t connection,
        hwc2_display_type_t type)
    : id(id),
      connection(connection),
      type(type),
      configs(),
      active_config(0),
      adf_intf_fd(adf_intf_fd),
      adf_dev(adf_dev) { }

hwc2_display::~hwc2_display()
{
    close(adf_intf_fd);
    adf_device_close(&adf_dev);
}

hwc2_error_t hwc2_display::set_connection(hwc2_connection_t connection)
{
    if (connection == HWC2_CONNECTION_INVALID) {
        ALOGE("dpy %" PRIu64 ": invalid connection", id);
        return HWC2_ERROR_BAD_PARAMETER;
    }

    this->connection = connection;
    return HWC2_ERROR_NONE;
}

int hwc2_display::retrieve_display_configs(struct adf_hwc_helper *adf_helper)
{
    size_t num_configs = 0;

    int ret = adf_getDisplayConfigs(adf_helper, id, nullptr, &num_configs);
    if (ret < 0 || num_configs == 0) {
        ALOGE("dpy %" PRIu64 ": failed to get display configs: %s", id,
                strerror(ret));
        return ret;
    }

    std::vector<uint32_t> config_handles(num_configs);

    ret = adf_getDisplayConfigs(adf_helper, id, config_handles.data(),
            &num_configs);
    if (ret < 0) {
        ALOGE("dpy %" PRIu64 ": failed to get display configs: %s", id,
                strerror(ret));
        return ret;
    }

    active_config = config_handles[0];

    std::array<uint32_t, 6> attributes = {{
            HWC2_ATTRIBUTE_WIDTH,
            HWC2_ATTRIBUTE_HEIGHT,
            HWC2_ATTRIBUTE_VSYNC_PERIOD,
            HWC2_ATTRIBUTE_DPI_X,
            HWC2_ATTRIBUTE_DPI_Y,
            HWC2_ATTRIBUTE_INVALID }};
    std::array<int32_t, 5> values;

    for (auto config_handle: config_handles) {
        ret = adf_getDisplayAttributes_v2(adf_helper, id, config_handle,
                attributes.data(), values.data());
        if (ret < 0) {
            ALOGW("dpy %" PRIu64 ": failed to get display attributes for config"
                    " %u: %s", id, config_handle, strerror(ret));
            continue;
        }

        configs.emplace(config_handle, hwc2_config());

        for (size_t attr = 0; attr < attributes.size() - 1; attr++) {
            ret = configs[config_handle].set_attribute(
                    static_cast<hwc2_attribute_t>(attributes[attr]),
                    values[attr]);
            if (ret < 0)
                ALOGW("dpy %" PRIu64 ": failed to set attribute", id);
        }
    }

    return ret;
}

hwc2_display_t hwc2_display::get_next_id()
{
    return display_cnt++;
}
