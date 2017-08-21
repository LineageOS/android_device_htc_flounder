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

#include <sstream>
#include <cstdlib>
#include <vector>

#include "hwc2.h"

#define ATRACE_TAG ATRACE_TAG_GRAPHICS
#include "cutils/trace.h"

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
    : state_mutex(),
      callback_handler(),
      displays(),
      dump_str(),
      adf_helper(nullptr) { }

hwc2_dev::~hwc2_dev()
{
    if (adf_helper)
        adf_hwc_close(adf_helper);
    hwc2_display::reset_ids();
}

std::string hwc2_dev::dump() const
{
    std::stringstream dmp;

    dmp << "NVIDIA HWC2:\n";
    for (auto &dpy: displays)
        dmp << dpy.second.dump() << "\n";

    return dmp.str();
}

void hwc2_dev::dump_hwc2(uint32_t *out_size, char *out_buffer)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    if (!out_buffer) {
        dump_str.clear();
        dump_str.append(dump());
        *out_size = dump_str.length();
        return;
    }

    *out_size = (*out_size > dump_str.length())? dump_str.length(): *out_size;
    dump_str.copy(out_buffer, *out_size);
    dump_str.clear();
}

hwc2_error_t hwc2_dev::get_display_name(hwc2_display_t dpy_id, uint32_t *out_size,
        char *out_name)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_name(out_size, out_name);
}

hwc2_error_t hwc2_dev::get_display_type(hwc2_display_t dpy_id,
        hwc2_display_type_t *out_type)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    *out_type = it->second.get_type();
    return HWC2_ERROR_NONE;
}

uint32_t hwc2_dev::get_max_virtual_display_count()
{
    return 0;
}

hwc2_error_t hwc2_dev::create_virtual_display(uint32_t /*width*/,
        uint32_t /*height*/, android_pixel_format_t* /*format*/,
        hwc2_display_t* /*out_display*/)
{
    return HWC2_ERROR_NO_RESOURCES;
}

hwc2_error_t hwc2_dev::destroy_virtual_display(hwc2_display_t dpy_id)
{
    if (displays.find(dpy_id) != displays.end())
        return HWC2_ERROR_BAD_PARAMETER;
    else
        return HWC2_ERROR_BAD_DISPLAY;
}

hwc2_error_t hwc2_dev::set_output_buffer(hwc2_display_t dpy_id,
        buffer_handle_t /*buffer*/, int32_t /*release_fence*/)
{
    if (displays.find(dpy_id) != displays.end())
        return HWC2_ERROR_UNSUPPORTED;
    else
        return HWC2_ERROR_BAD_DISPLAY;
}

hwc2_error_t hwc2_dev::set_power_mode(hwc2_display_t dpy_id,
        hwc2_power_mode_t mode)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.set_power_mode(mode);
}

hwc2_error_t hwc2_dev::get_doze_support(hwc2_display_t dpy_id,
        int32_t *out_support)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_doze_support(out_support);
}

hwc2_error_t hwc2_dev::validate_display(hwc2_display_t dpy_id,
        uint32_t *out_num_types, uint32_t *out_num_requests)
{
    ATRACE_BEGIN(__func__);

    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    hwc2_error_t ret = it->second.validate_display(out_num_types,
            out_num_requests);

    ATRACE_END();

    return ret;
}

hwc2_error_t hwc2_dev::get_changed_composition_types(hwc2_display_t dpy_id,
        uint32_t *out_num_elements, hwc2_layer_t *out_layers,
        hwc2_composition_t *out_types)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_changed_composition_types(out_num_elements,
            out_layers, out_types);
}

hwc2_error_t hwc2_dev::get_display_requests(hwc2_display_t dpy_id,
        hwc2_display_request_t *out_display_requests,
        uint32_t *out_num_elements, hwc2_layer_t *out_layers,
        hwc2_layer_request_t *out_layer_requests)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_display_requests(out_display_requests,
            out_num_elements, out_layers, out_layer_requests);
}

hwc2_error_t hwc2_dev::accept_display_changes(hwc2_display_t dpy_id)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.accept_display_changes();
}

hwc2_error_t hwc2_dev::present_display(hwc2_display_t dpy_id,
        int32_t *out_present_fence)
{
    ATRACE_BEGIN(__func__);

    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    hwc2_error_t ret = it->second.present_display(out_present_fence);

    ATRACE_END();

    return ret;
}

hwc2_error_t hwc2_dev::get_release_fences(hwc2_display_t dpy_id,
        uint32_t *out_num_elements, hwc2_layer_t *out_layers,
        int32_t *out_fences)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_release_fences(out_num_elements, out_layers,
            out_fences);
}

hwc2_error_t hwc2_dev::get_display_attribute(hwc2_display_t dpy_id,
        hwc2_config_t config, hwc2_attribute_t attribute, int32_t *out_value)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_display_attribute(config, attribute, out_value);
}

hwc2_error_t hwc2_dev::get_display_configs(hwc2_display_t dpy_id,
        uint32_t *out_num_configs, hwc2_config_t *out_configs)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_display_configs(out_num_configs, out_configs);
}

hwc2_error_t hwc2_dev::get_active_config(hwc2_display_t dpy_id,
        hwc2_config_t *out_config)
{
    std::lock_guard<std::mutex> guard(state_mutex);

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
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.set_active_config(adf_helper, config);
}

hwc2_error_t hwc2_dev::get_color_modes(hwc2_display_t dpy_id,
        uint32_t *out_num_modes, android_color_mode_t *out_modes)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_color_modes(out_num_modes, out_modes);
}

hwc2_error_t hwc2_dev::set_color_mode(hwc2_display_t dpy_id,
        android_color_mode_t mode)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.set_color_mode(mode);
}

hwc2_error_t hwc2_dev::get_hdr_capabilities(hwc2_display_t dpy_id,
        uint32_t *out_num_types, android_hdr_t *out_types,
        float *out_max_luminance, float *out_max_average_luminance,
        float *out_min_luminance)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_hdr_capabilities(out_num_types, out_types,
            out_max_luminance, out_max_average_luminance,
            out_min_luminance);
}

hwc2_error_t hwc2_dev::set_color_transform(hwc2_display_t dpy_id,
        const float *color_matrix, android_color_transform_t color_hint)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.set_color_transform(color_matrix, color_hint);
}

hwc2_error_t hwc2_dev::get_client_target_support(hwc2_display_t dpy_id,
        uint32_t width, uint32_t height, android_pixel_format_t format,
        android_dataspace_t dataspace)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_client_target_support(width, height, format,
            dataspace);
}

hwc2_error_t hwc2_dev::set_client_target(hwc2_display_t dpy_id,
        buffer_handle_t target, int32_t acquire_fence,
        android_dataspace_t dataspace, const hwc_region_t &surface_damage)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.set_client_target(target, acquire_fence, dataspace,
            surface_damage);
}

hwc2_error_t hwc2_dev::create_layer(hwc2_display_t dpy_id, hwc2_layer_t *out_layer)
{
    std::lock_guard<std::mutex> guard(state_mutex);

    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.create_layer(out_layer);
}

hwc2_error_t hwc2_dev::destroy_layer(hwc2_display_t dpy_id, hwc2_layer_t lyr_id)
{
    std::lock_guard<std::mutex> guard(state_mutex);

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
    std::lock_guard<std::mutex> guard(state_mutex);
    return displays.find(dpy_id)->second.set_layer_composition_type(lyr_id,
            comp_type);
}

hwc2_error_t hwc2_dev::set_layer_buffer(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, buffer_handle_t handle, int32_t acquire_fence)
{
    std::lock_guard<std::mutex> guard(state_mutex);
    return displays.find(dpy_id)->second.set_layer_buffer(lyr_id, handle,
            acquire_fence);
}

hwc2_error_t hwc2_dev::set_layer_dataspace(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, android_dataspace_t dataspace)
{
    std::lock_guard<std::mutex> guard(state_mutex);
    return displays.find(dpy_id)->second.set_layer_dataspace(lyr_id, dataspace);
}

hwc2_error_t hwc2_dev::set_layer_display_frame(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, const hwc_rect_t &display_frame)
{
    std::lock_guard<std::mutex> guard(state_mutex);
    return displays.find(dpy_id)->second.set_layer_display_frame(lyr_id,
            display_frame);
}

hwc2_error_t hwc2_dev::set_layer_source_crop(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, const hwc_frect_t &source_crop)
{
    std::lock_guard<std::mutex> guard(state_mutex);
    return displays.find(dpy_id)->second.set_layer_source_crop(lyr_id,
            source_crop);
}

hwc2_error_t hwc2_dev::set_layer_z_order(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, uint32_t z_order)
{
    std::lock_guard<std::mutex> guard(state_mutex);
    return displays.find(dpy_id)->second.set_layer_z_order(lyr_id, z_order);
}

hwc2_error_t hwc2_dev::set_layer_surface_damage(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, const hwc_region_t &surface_damage)
{
    std::lock_guard<std::mutex> guard(state_mutex);
    return displays.find(dpy_id)->second.set_layer_surface_damage(lyr_id,
            surface_damage);
}

hwc2_error_t hwc2_dev::set_layer_blend_mode(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, hwc2_blend_mode_t blend_mode)
{
    std::lock_guard<std::mutex> guard(state_mutex);
    return displays.find(dpy_id)->second.set_layer_blend_mode(lyr_id,
            blend_mode);
}

hwc2_error_t hwc2_dev::set_layer_plane_alpha(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, float plane_alpha)
{
    std::lock_guard<std::mutex> guard(state_mutex);
    return displays.find(dpy_id)->second.set_layer_plane_alpha(lyr_id,
            plane_alpha);
}

hwc2_error_t hwc2_dev::set_layer_transform(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, const hwc_transform_t transform)
{
    std::lock_guard<std::mutex> guard(state_mutex);
    return displays.find(dpy_id)->second.set_layer_transform(lyr_id, transform);
}

hwc2_error_t hwc2_dev::set_layer_visible_region(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, const hwc_region_t &visible_region)
{
    std::lock_guard<std::mutex> guard(state_mutex);
    return displays.find(dpy_id)->second.set_layer_visible_region(lyr_id,
            visible_region);
}

hwc2_error_t hwc2_dev::set_layer_color(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, const hwc_color_t &color)
{
    return displays.find(dpy_id)->second.set_layer_color(lyr_id, color);
}

hwc2_error_t hwc2_dev::set_cursor_position(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, int32_t x, int32_t y)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return displays.find(dpy_id)->second.set_cursor_position(lyr_id, x, y);
}

void hwc2_dev::hotplug(hwc2_display_t dpy_id, hwc2_connection_t connection)
{
    {
        std::lock_guard<std::mutex> guard(state_mutex);

        auto it = displays.find(dpy_id);
        if (it == displays.end()) {
            ALOGW("dpy %" PRIu64 ": invalid display handle preventing hotplug"
                    " callback", dpy_id);
            return;
        }

        hwc2_error_t ret = it->second.set_connection(connection);
        if (ret != HWC2_ERROR_NONE)
            return;
    }

    callback_handler.call_hotplug(dpy_id, connection);
}

void hwc2_dev::vsync(hwc2_display_t dpy_id, uint64_t timestamp)
{
    {
        std::lock_guard<std::mutex> guard(state_mutex);

        auto it = displays.find(dpy_id);
        if (it == displays.end()) {
            ALOGW("dpy %" PRIu64 ": invalid display handle preventing vsync"
                    " callback", dpy_id);
            return;
        }
    }

    callback_handler.call_vsync(dpy_id, timestamp);
}

hwc2_error_t hwc2_dev::set_vsync_enabled(hwc2_display_t dpy_id,
        hwc2_vsync_t enabled)
{
    std::lock_guard<std::mutex> guard(state_mutex);

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

    std::lock_guard<std::mutex> guard(state_mutex);

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
        dpy.second.set_client_target_properties();
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
