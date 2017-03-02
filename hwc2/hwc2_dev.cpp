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

#include <fcntl.h>
#include <cutils/log.h>
#include <inttypes.h>
#include <cstdlib>
#include <vector>

#include "hwc2.h"

static void hwc2_vsync(void *data, int dpy_id, uint64_t timestamp)
{
    hwc2_dev *dev = static_cast<hwc2_dev *>(data);
    dev->vsync(static_cast<hwc2_display_t>(dpy_id), timestamp);
}

static void hwc2_hotplug(void *data, int dpy_id, bool connected)
{
    hwc2_dev *dev = static_cast<hwc2_dev *>(data);
    dev->hotplug(static_cast<hwc2_display_t>(dpy_id),
            (connected)? HWC2_CONNECTION_CONNECTED:
            HWC2_CONNECTION_DISCONNECTED);
}

static void hwc2_custom_event(void* /*data*/, int /*dpy_id*/,
        struct adf_event* /*event*/)
{
    return;
}

const struct adf_hwc_event_callbacks hwc2_adfhwc_callbacks = {
    .vsync = hwc2_vsync,
    .hotplug = hwc2_hotplug,
    .custom_event = hwc2_custom_event,
};

hwc2_dev::hwc2_dev()
    : callback_handler(),
      displays(),
      adf_helper(nullptr) { }

hwc2_dev::~hwc2_dev()
{
    if (adf_helper)
        adf_hwc_close(adf_helper);
    hwc2_display::reset_ids();
}

hwc2_error_t hwc2_dev::get_display_name(hwc2_display_t dpy_id, uint32_t *out_size,
        char *out_name) const
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_name(out_size, out_name);
}

hwc2_error_t hwc2_dev::get_display_type(hwc2_display_t dpy_id,
        hwc2_display_type_t *out_type) const
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    *out_type = it->second.get_type();
    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_dev::set_power_mode(hwc2_display_t dpy_id,
        hwc2_power_mode_t mode)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.set_power_mode(mode);
}

hwc2_error_t hwc2_dev::get_doze_support(hwc2_display_t dpy_id,
        int32_t *out_support) const
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_doze_support(out_support);
}

hwc2_error_t hwc2_dev::get_display_attribute(hwc2_display_t dpy_id,
        hwc2_config_t config, hwc2_attribute_t attribute, int32_t *out_value)
        const
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_display_attribute(config, attribute, out_value);
}

hwc2_error_t hwc2_dev::get_display_configs(hwc2_display_t dpy_id,
        uint32_t *out_num_configs, hwc2_config_t *out_configs) const
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_display_configs(out_num_configs, out_configs);
}

hwc2_error_t hwc2_dev::get_active_config(hwc2_display_t dpy_id,
        hwc2_config_t *out_config) const
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_active_config(out_config);
}

hwc2_error_t hwc2_dev::set_active_config(hwc2_display_t dpy_id,
        hwc2_config_t config)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.set_active_config(adf_helper, config);
}

hwc2_error_t hwc2_dev::create_layer(hwc2_display_t dpy_id, hwc2_layer_t *out_layer)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.create_layer(out_layer);
}

hwc2_error_t hwc2_dev::destroy_layer(hwc2_display_t dpy_id, hwc2_layer_t lyr_id)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.destroy_layer(lyr_id);
}

hwc2_error_t hwc2_dev::set_layer_composition_type(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, hwc2_composition_t comp_type)
{
    return displays.find(dpy_id)->second.set_layer_composition_type(lyr_id,
            comp_type);
}

hwc2_error_t hwc2_dev::set_layer_dataspace(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, android_dataspace_t dataspace)
{
    return displays.find(dpy_id)->second.set_layer_dataspace(lyr_id, dataspace);
}

hwc2_error_t hwc2_dev::set_layer_blend_mode(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, hwc2_blend_mode_t blend_mode)
{
    return displays.find(dpy_id)->second.set_layer_blend_mode(lyr_id,
            blend_mode);
}

void hwc2_dev::hotplug(hwc2_display_t dpy_id, hwc2_connection_t connection)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGW("dpy %" PRIu64 ": invalid display handle preventing hotplug"
                " callback", dpy_id);
        return;
    }

    hwc2_error_t ret = it->second.set_connection(connection);
    if (ret != HWC2_ERROR_NONE)
        return;

    callback_handler.call_hotplug(dpy_id, connection);
}

void hwc2_dev::vsync(hwc2_display_t dpy_id, uint64_t timestamp)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGW("dpy %" PRIu64 ": invalid display handle preventing vsync"
                " callback", dpy_id);
        return;
    }

    callback_handler.call_vsync(dpy_id, timestamp);
}

hwc2_error_t hwc2_dev::set_vsync_enabled(hwc2_display_t dpy_id,
        hwc2_vsync_t enabled)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    if (it->second.get_type() != HWC2_DISPLAY_TYPE_PHYSICAL)
        return HWC2_ERROR_NONE;

    if (it->second.get_vsync_enabled() == enabled)
        return HWC2_ERROR_NONE;

    bool adf_enabled;
    switch (enabled) {
    case HWC2_VSYNC_ENABLE:
        adf_enabled = true;
        break;
    case HWC2_VSYNC_DISABLE:
        adf_enabled = false;
        break;
    default:
        ALOGW("dpy %" PRIu64 ": invalid vsync enable parameter %u", dpy_id,
                enabled);
        return HWC2_ERROR_BAD_PARAMETER;
    }

    int ret = adf_eventControl(adf_helper, dpy_id, HWC_EVENT_VSYNC, adf_enabled);
    if (ret < 0) {
        ALOGW("dpy %" PRIu64 ": failed to set vsync enabled: %s", dpy_id,
                strerror(ret));
        return HWC2_ERROR_BAD_PARAMETER;
    }

    return it->second.set_vsync_enabled(enabled);
}

hwc2_error_t hwc2_dev::register_callback(hwc2_callback_descriptor_t descriptor,
        hwc2_callback_data_t callback_data, hwc2_function_pointer_t pointer)
{
    if (descriptor == HWC2_CALLBACK_INVALID) {
        ALOGE("invalid callback descriptor %u", descriptor);
        return HWC2_ERROR_BAD_PARAMETER;
    }

    return callback_handler.register_callback(descriptor, callback_data,
            pointer);
}

int hwc2_dev::open_adf_device()
{
    adf_id_t *dev_ids = nullptr;
    int ret;

    ssize_t n_devs = adf_devices(&dev_ids);
    if (n_devs < 0) {
        ALOGE("failed to enumerate adf devices: %s", strerror(n_devs));
        return n_devs;
    }

    std::vector<int> intf_fds;

    for (ssize_t idx = 0; idx < n_devs; idx++) {
        int intf_fd = open_adf_display(dev_ids[idx]);
        if (intf_fd >= 0)
            intf_fds.push_back(intf_fd);
    }

    if (displays.empty()) {
        ALOGE("failed to open any physical displays");
        ret = -EINVAL;
        goto err_dpy_open;
    }

    ret = adf_hwc_open(intf_fds.data(), intf_fds.size(),
            &hwc2_adfhwc_callbacks, this, &adf_helper);
    if (ret < 0) {
        ALOGE("failed to open adf helper: %s", strerror(ret));
        goto err_hwc_open;
    }

    for (auto &dpy: displays) {
        ret = dpy.second.retrieve_display_configs(adf_helper);
        if (ret < 0) {
            ALOGE("dpy %" PRIu64 ": failed to retrieve display configs: %s",
                    dpy.second.get_id(), strerror(ret));
            goto err_rtrv;
        }
    }

    for (auto &dpy: displays)
        callback_handler.call_hotplug(dpy.second.get_id(),
                dpy.second.get_connection());

    free(dev_ids);
    return 0;

err_rtrv:
    adf_hwc_close(adf_helper);
err_hwc_open:
    displays.clear();
err_dpy_open:
    free(dev_ids);
    return ret;
}

int hwc2_dev::open_adf_display(adf_id_t adf_id) {
    struct adf_device adf_dev;

    int ret = adf_device_open(adf_id, O_RDWR, &adf_dev);
    if (ret < 0) {
        ALOGE("failed to open adf%u device: %s", adf_id, strerror(ret));
        return ret;
    }

    int intf_fd = adf_interface_open(&adf_dev, 0, O_RDWR);
    if (intf_fd < 0) {
        ALOGE("failed to open adf%u interface: %s", adf_id, strerror(intf_fd));
        adf_device_close(&adf_dev);
        return intf_fd;
    }

    struct adf_interface_data intf;
    ret = adf_get_interface_data(intf_fd, &intf);
    if (ret < 0) {
        ALOGE("failed to get adf%u interface data: %s", adf_id, strerror(ret));
        close(intf_fd);
        adf_device_close(&adf_dev);
        return ret;
    }

    hwc2_display_t dpy_id = hwc2_display::get_next_id();
    displays.emplace(std::piecewise_construct, std::forward_as_tuple(dpy_id),
            std::forward_as_tuple(dpy_id, intf_fd, adf_dev,
            (intf.hotplug_detect)? HWC2_CONNECTION_CONNECTED:
            HWC2_CONNECTION_DISCONNECTED, HWC2_DISPLAY_TYPE_PHYSICAL,
            (intf.hotplug_detect)? HWC2_POWER_MODE_ON: HWC2_POWER_MODE_OFF));

    adf_free_interface_data(&intf);

    return intf_fd;
}
