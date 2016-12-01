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
        hwc2_display_type_t type, hwc2_power_mode_t power_mode)
    : id(id),
      name(),
      connection(connection),
      type(type),
      layers(),
      vsync_enabled(HWC2_VSYNC_DISABLE),
      configs(),
      active_config(0),
      power_mode(power_mode),
      adf_intf_fd(adf_intf_fd),
      adf_dev(adf_dev)
{
    init_name();
}

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

hwc2_error_t hwc2_display::get_name(uint32_t *out_size, char *out_name) const
{
    if (!out_name) {
        *out_size = name.size();
        return HWC2_ERROR_NONE;
    }

    /* out_name does not require a NULL terminator so strncpy can truncate
     * the output safely */
    strncpy(out_name, name.c_str(), *out_size);
    *out_size = (*out_size < name.size())? *out_size: name.size();
    return HWC2_ERROR_NONE;
}

void hwc2_display::init_name()
{
    name.append("dpy-");
    if (HWC2_DISPLAY_TYPE_PHYSICAL)
        name.append("phys-");
    else
        name.append("virt-");
    name.append(std::to_string(id));
}

hwc2_error_t hwc2_display::set_power_mode(hwc2_power_mode_t mode)
{
    int drm_mode;

    switch (mode) {
    case HWC2_POWER_MODE_ON:
        drm_mode = DRM_MODE_DPMS_ON;
        break;
    case HWC2_POWER_MODE_OFF:
        drm_mode = DRM_MODE_DPMS_OFF;
        break;
    case HWC2_POWER_MODE_DOZE:
    case HWC2_POWER_MODE_DOZE_SUSPEND:
        ALOGE("dpy %" PRIu64 ": unsupported power mode: %u", id, mode);
        return HWC2_ERROR_UNSUPPORTED;
    default:
        ALOGE("dpy %" PRIu64 ": invalid power mode: %u", id, mode);
        return HWC2_ERROR_BAD_PARAMETER;
    }

    adf_interface_blank(adf_intf_fd, drm_mode);
    power_mode = mode;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::get_doze_support(int32_t *out_support) const
{
    *out_support = 0;
    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::set_vsync_enabled(hwc2_vsync_t enabled)
{
    if (enabled == HWC2_VSYNC_INVALID) {
        ALOGE("dpy %" PRIu64 ": invalid vsync enabled", id);
        return HWC2_ERROR_BAD_PARAMETER;
    }

    this->vsync_enabled = enabled;
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
        ret = adf_getDisplayAttributes_hwc2(adf_helper, id, config_handle,
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

hwc2_error_t hwc2_display::get_display_attribute(hwc2_config_t config,
        hwc2_attribute_t attribute, int32_t *out_value) const
{
    auto it = configs.find(config);
    if (it == configs.end()) {
        ALOGE("dpy %" PRIu64 ": bad config", id);
        return HWC2_ERROR_BAD_CONFIG;
    }

    *out_value = it->second.get_attribute(attribute);
    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::get_display_configs(uint32_t *out_num_configs,
        hwc2_config_t *out_configs) const
{
    if (!out_configs) {
        *out_num_configs = configs.size();
        return HWC2_ERROR_NONE;
    }

    size_t idx = 0;
    for (auto it = configs.begin(); it != configs.end()
            && idx < *out_num_configs; it++, idx++)
        out_configs[idx] = it->first;

    *out_num_configs = idx;
    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::get_active_config(hwc2_config_t *out_config) const
{
    if (!configs.size()) {
        ALOGE("dpy %" PRIu64 ": no active config", id);
        return HWC2_ERROR_BAD_CONFIG;
    }

    *out_config = active_config;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::set_active_config(
        struct adf_hwc_helper *adf_helper, hwc2_config_t config)
{
    if (config >= configs.size()) {
        ALOGE("dpy %" PRIu64 ": bad config", id);
        return HWC2_ERROR_BAD_CONFIG;
    }

    int ret = adf_set_active_config_hwc2(adf_helper, id, config);
    if (ret < 0) {
        ALOGE("dpy %" PRIu64 ": failed to set mode: %s", id, strerror(ret));
        return HWC2_ERROR_BAD_CONFIG;
    }

    active_config = config;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::create_layer(hwc2_layer_t *out_layer)
{
    hwc2_layer_t lyr_id = hwc2_layer::get_next_id();
    layers.emplace(std::piecewise_construct, std::forward_as_tuple(lyr_id),
            std::forward_as_tuple(lyr_id));

    *out_layer = lyr_id;
    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::destroy_layer(hwc2_layer_t lyr_id)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    layers.erase(lyr_id);
    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::set_layer_composition_type(hwc2_layer_t lyr_id,
        hwc2_composition_t comp_type)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    return it->second.set_comp_type(comp_type);
}

hwc2_error_t hwc2_display::set_layer_dataspace(hwc2_layer_t lyr_id,
        android_dataspace_t dataspace)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    return it->second.set_dataspace(dataspace);
}

hwc2_error_t hwc2_display::set_layer_display_frame(hwc2_layer_t lyr_id,
        const hwc_rect_t &display_frame)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    return it->second.set_display_frame(display_frame);
}

hwc2_error_t hwc2_display::set_layer_source_crop(hwc2_layer_t lyr_id,
        const hwc_frect_t &source_crop)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    return it->second.set_source_crop(source_crop);
}

hwc2_error_t hwc2_display::set_layer_z_order(hwc2_layer_t lyr_id, uint32_t z_order)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    return it->second.set_z_order(z_order);
}

hwc2_error_t hwc2_display::set_layer_surface_damage(hwc2_layer_t lyr_id,
        const hwc_region_t &surface_damage)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    return it->second.set_surface_damage(surface_damage);
}

hwc2_error_t hwc2_display::set_layer_blend_mode(hwc2_layer_t lyr_id,
        hwc2_blend_mode_t blend_mode)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    return it->second.set_blend_mode(blend_mode);
}

hwc2_error_t hwc2_display::set_layer_plane_alpha(hwc2_layer_t lyr_id, float plane_alpha)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    return it->second.set_plane_alpha(plane_alpha);
}

hwc2_error_t hwc2_display::set_layer_transform(hwc2_layer_t lyr_id,
        const hwc_transform_t transform)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    return it->second.set_transform(transform);
}

hwc2_error_t hwc2_display::set_layer_color(hwc2_layer_t lyr_id,
        const hwc_color_t& /*color*/)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    /* Layer colors are not supported on flounder. During validate, any layers
     * marked HWC2_COMPOSITION_SOLID_COLOR will be changed to
     * HWC2_COMPOSITION_CLIENT. No need to store the layer color. */
    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::set_cursor_position(hwc2_layer_t lyr_id,
        int32_t /*x*/, int32_t /*y*/)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    /* Cursors are not supported on flounder. During validate, any layers marked
     * HWC2_COMPOSITION_CURSOR will be changed to HWC2_COMPOSITION_CLIENT.
     * No need to store the cursor position. */
    return HWC2_ERROR_NONE;
}

hwc2_display_t hwc2_display::get_next_id()
{
    return display_cnt++;
}
