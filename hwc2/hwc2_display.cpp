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
#include <tegra_adf.h>
#include <inttypes.h>

#include <sstream>
#include <cstdlib>
#include <map>
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
      display_state(modified),
      client_target_used(false),
      windows(),
      client_target(),
      layers(),
      vsync_enabled(HWC2_VSYNC_DISABLE),
      changed_comp_types(),
      configs(),
      active_config(0),
      power_mode(power_mode),
      color_matrix(),
      color_hint(HAL_COLOR_TRANSFORM_IDENTITY),
      release_fence(-1),
      adf_intf_fd(adf_intf_fd),
      adf_dev(adf_dev)
{
    init_name();
    init_windows();
}

hwc2_display::~hwc2_display()
{
    close(adf_intf_fd);
    adf_device_close(&adf_dev);
}

std::string hwc2_display::dump() const
{
    std::stringstream dmp;

    dmp << "Display [" << id << "] " << getDisplayTypeName(type) << ":\n";

    dmp << "  Power Mode: " << getPowerModeName(power_mode) << "\n";

    dmp << "  Active Config:";

    auto it = configs.begin();
    if (configs.size() > 0 || it != configs.end())
        dmp << "\n" << configs.at(active_config).dump();
    else
        dmp << " None\n";

    if (power_mode == HWC2_POWER_MODE_OFF)
        return dmp.str();

    size_t idx = 0;
    for (auto &win: windows) {
        dmp << "  Window [" << idx << "]:";

        if (win.contains_layer()) {
            dmp << " Layer\n";
            dmp << layers.at(win.get_layer()).dump();

        } else if (win.contains_client_target()) {
            dmp << " Client Target\n";
            dmp << client_target.dump();

        } else
            dmp << " Unused\n";

        idx++;
    }

    return dmp.str();
}

hwc2_error_t hwc2_display::set_connection(hwc2_connection_t connection)
{
    if (connection == HWC2_CONNECTION_INVALID) {
        ALOGE("dpy %" PRIu64 ": invalid connection", id);
        return HWC2_ERROR_BAD_PARAMETER;
    }

    display_state = modified;
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

hwc2_error_t hwc2_display::validate_display(uint32_t *out_num_types,
        uint32_t *out_num_requests)
{
    if (display_state == valid) {
        *out_num_types = 0;
        *out_num_requests = 0;
        return HWC2_ERROR_NONE;
    }

    clear_windows();
    changed_comp_types.clear();

    if (color_hint != HAL_COLOR_TRANSFORM_IDENTITY)
        force_client_composition();
    else
        assign_composition();

    *out_num_requests = 0;
    *out_num_types = changed_comp_types.size();

    for (auto &lyr: layers)
        lyr.second.set_modified(false);

    if (*out_num_types > 0) {
        display_state = invalid;
        return HWC2_ERROR_HAS_CHANGES;
    }

    display_state = valid;
    return HWC2_ERROR_NONE;
}

void hwc2_display::force_client_composition()
{
    for (auto &lyr: layers) {
        hwc2_composition_t comp_type = lyr.second.get_comp_type();
        if (comp_type != HWC2_COMPOSITION_CLIENT)
            changed_comp_types.emplace(lyr.second.get_id(), comp_type);
    }
}

void hwc2_display::assign_composition()
{
    hwc2_error_t ret;

    std::map<uint32_t, hwc2_layer_t> ordered_layers;
    for (auto &lyr: layers)
        ordered_layers.emplace(lyr.second.get_z_order(), lyr.second.get_id());

    bool client_target_assigned = false;

    /* If there will definitely be a client target, assign it to the front most
     * window */
    if (layers.size() > windows.size()) {
        ret = assign_client_target_window(0);
        ALOG_ASSERT(ret == HWC2_ERROR_NONE, "No valid client target window");

        client_target_assigned = true;
    }

    bool retry_assignment;

    do {
        retry_assignment = false;
        /* The display controller uses a different z order than SurfaceFlinger */
        uint32_t z_order = windows.size() - 1;
        client_target_used = false;

        /* Iterate over the layers from the back to the front.
         * assign windows to device layers. Upon reaching a layer that must
         * be composed by the client all subsequent layers will be assigned
         * client composition */
        for (auto &ordered_layer: ordered_layers) {
            hwc2_layer_t lyr_id = ordered_layer.second;
            hwc2_composition_t comp_type = layers.find(
                    lyr_id)->second.get_comp_type();

            if (comp_type == HWC2_COMPOSITION_INVALID) {
                ALOGW("dpy %" PRIu64 " lyr %" PRIu64 ": invalid composition"
                        " type %u", id, lyr_id, comp_type);
                continue;
            }

            /* If the layer is not overlapped and can be assigned a window,
               assign it and continue to the next layer */
            if (comp_type == HWC2_COMPOSITION_DEVICE && !client_target_used
                    && assign_layer_window(z_order, lyr_id) == HWC2_ERROR_NONE) {
                z_order--;
                continue;
            }

            /* The layer will be composited into a client target buffer so
             * find a window for the client target */
            if (!client_target_assigned) {

                ret = assign_client_target_window(0);
                if (ret != HWC2_ERROR_NONE) {
                    /* If no client target can be assigned, clear all the
                     * windows, assign a client target and retry assigning
                     * windows */
                    retry_assignment = true;

                    changed_comp_types.clear();
                    clear_windows();

                    ret = assign_client_target_window(0);
                    ALOG_ASSERT(ret == HWC2_ERROR_NONE, "No valid client target"
                            " window");

                    client_target_assigned = true;
                    break;
                }

                client_target_assigned = true;
            }

            /* Assign the layer to client composition */
            if (comp_type != HWC2_COMPOSITION_CLIENT)
                changed_comp_types.emplace(lyr_id, HWC2_COMPOSITION_CLIENT);

            client_target_used = true;
        }
    } while (retry_assignment);
}

hwc2_error_t hwc2_display::get_changed_composition_types(
        uint32_t *out_num_elements, hwc2_layer_t *out_layers,
        hwc2_composition_t *out_types) const
{
    if (display_state == modified) {
        ALOGE("dpy %" PRIu64 ": display has been modified since last call to"
                " validate_display", id);
        return HWC2_ERROR_NOT_VALIDATED;
    }

    if (!out_layers || !out_types) {
        *out_num_elements = changed_comp_types.size();
        return HWC2_ERROR_NONE;
    }

    size_t idx = 0;
    for (auto &changed: changed_comp_types) {
        out_layers[idx] = changed.first;
        out_types[idx] = changed.second;
        idx++;
    }

    *out_num_elements = changed_comp_types.size();
    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::get_display_requests(
        hwc2_display_request_t *out_display_requests,
        uint32_t *out_num_elements, hwc2_layer_t* /*out_layers*/,
        hwc2_layer_request_t* /*out_layer_requests*/) const
{
    if (display_state == modified) {
        ALOGE("dpy %" PRIu64 ": display has been modified since last call to"
                " validate_display", id);
        return HWC2_ERROR_NOT_VALIDATED;
    }

    *out_display_requests = static_cast<hwc2_display_request_t>(0);
    *out_num_elements = 0;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::accept_display_changes()
{
    if (display_state == modified) {
        ALOGE("dpy %" PRIu64 ": display has been modified since last call to"
                " validate_display", id);
        return HWC2_ERROR_NOT_VALIDATED;
    }

    for (auto &changed: changed_comp_types)
        layers.find(changed.first)->second.set_comp_type(changed.second);

    display_state = valid;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::present_display(int32_t *out_present_fence)
{
    std::vector<struct adf_buffer_config> adf_bufs(windows.size());
    std::array<adf_id_t, 1> interfaces = {{0}};
    int new_release_fence = -1, err;

    hwc2_error_t ret = prepare_present_display();
    if (ret != HWC2_ERROR_NONE) {
        ALOGE("dpy %" PRIu64 ": failed to prepare display for presenting", id);
        *out_present_fence = -1;
        return ret;
    }

    tegra_adf_flip *args;
    size_t args_size = sizeof(*args) + windows.size() * sizeof(args->win[0]);

    args = static_cast<tegra_adf_flip *>(calloc(1, args_size));
    if (!args) {
        ALOGE("dpy %" PRIu64 ": failed to alloc tegra_adf_flip", id);
        *out_present_fence = -1;
        return HWC2_ERROR_NO_RESOURCES;
    }

    args->win_num = windows.size();

    size_t win_idx = 0, buf_idx = 0;
    for (auto &win: windows) {
        if (win.contains_client_target()) {

            ret = client_target.get_adf_post_props(&args->win[win_idx],
                    &adf_bufs[buf_idx], win_idx, buf_idx, win.get_z_order());
            if (ret != HWC2_ERROR_NONE) {
                ALOGE("dpy %" PRIu64 ": failed to get client target adf props", id);
                goto done;
            }
            buf_idx++;

        } else if (win.contains_layer()) {
            hwc2_layer_t lyr_id = win.get_layer();

            ret = layers.find(lyr_id)->second.get_adf_post_props(
                    &args->win[win_idx], &adf_bufs[buf_idx], win_idx,
                    buf_idx, win.get_z_order());
            if (ret != HWC2_ERROR_NONE) {
                ALOGE("dpy %" PRIu64 " lyr %" PRIu64 ": failed to get layer adf"
                        " props", id, lyr_id);
                goto done;
            }
            buf_idx++;

        } else {
            struct tegra_adf_flip_windowattr *win_attr = &args->win[win_idx];
            win_attr->win_index = win_idx;
            win_attr->buf_index = -1;
            win_attr->z = win.get_z_order();
            win_attr->blend = TEGRA_ADF_BLEND_NONE;
            win_attr->flags = 0;
        }

        win_idx++;
    }

    err = adf_device_post_v2(&adf_dev, interfaces.data(), interfaces.size(),
            adf_bufs.data(), buf_idx, args, args_size, ADF_COMPLETE_FENCE_PRESENT,
            &new_release_fence);
    if (err < 0) {
        ALOGE("dpy %" PRIu64 ": adf_device_post_v2 failed %s", id, strerror(err));
        err = HWC2_ERROR_NO_RESOURCES;
        new_release_fence = -1;
    }

    release_fence.reset(new_release_fence);

    close_acquire_fences();

    for (size_t idx = 0; idx < buf_idx; idx++)
        if (adf_bufs[idx].fd[0] >= 0)
            close(adf_bufs[idx].fd[0]);

done:
    free(args);
    *out_present_fence = dup(release_fence.get());
    return ret;
}

hwc2_error_t hwc2_display::prepare_present_display()
{
    if (display_state != valid) {
        ALOGE("dpy %" PRIu64 ": display not validated: %d", id, display_state);
        return HWC2_ERROR_NOT_VALIDATED;
    }

    if (connection != HWC2_CONNECTION_CONNECTED) {
        ALOGW("dpy %" PRIu64 ": invalid connection: %d", id, connection);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    if (power_mode == HWC2_POWER_MODE_OFF) {
        ALOGW("dpy %" PRIu64 ": invalid power mode: %d", id, power_mode);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    hwc2_error_t ret = decompress_window_buffers();
    if (ret != HWC2_ERROR_NONE) {
        ALOGE("dpy %" PRIu64 ": failed to decompress buffers", id);
        return ret;
    }

    return HWC2_ERROR_NONE;
}


void hwc2_display::close_acquire_fences()
{
    for (auto &lyr: layers)
        lyr.second.close_acquire_fence();

    if (client_target_used)
        client_target.close_acquire_fence();
}

hwc2_error_t hwc2_display::get_release_fences(uint32_t *out_num_elements,
        hwc2_layer_t *out_layers, int32_t *out_fences) const
{
    if (release_fence.get() < 0) {
        *out_num_elements = 0;
        return HWC2_ERROR_NONE;
    }

    size_t num = 0;

    if (!out_layers || !out_fences) {
        for (auto &window: windows)
            if (window.contains_layer())
                num++;
        *out_num_elements = num;
        return HWC2_ERROR_NONE;
    }

    for (auto it = windows.begin(); num < *out_num_elements,
            it != windows.end(); it++) {
        if (it->contains_layer()) {
            out_layers[num] = it->get_layer();
            out_fences[num] = dup(release_fence.get());
            num++;
        }
    }

    *out_num_elements = num;
    return HWC2_ERROR_NONE;
}

void hwc2_display::init_windows()
{
    for (auto it = windows.begin(); it != windows.end(); it++)
        it->set_capabilities(window_capabilities[it - windows.begin()]);
}

void hwc2_display::clear_windows()
{
    for (auto &window: windows)
        window.clear();
}

hwc2_error_t hwc2_display::assign_client_target_window(uint32_t z_order)
{
    for (auto &window: windows)
        if (window.assign_client_target(z_order) == HWC2_ERROR_NONE)
            return HWC2_ERROR_NONE;

    return HWC2_ERROR_NO_RESOURCES;
}

hwc2_error_t hwc2_display::assign_layer_window(uint32_t z_order,
        hwc2_layer_t lyr_id)
{
    auto& lyr = layers.find(lyr_id)->second;

    if (!hwc2_window::is_supported(lyr))
        return HWC2_ERROR_UNSUPPORTED;

    for (auto &window: windows)
        if (window.assign_layer(z_order, lyr) == HWC2_ERROR_NONE)
            return HWC2_ERROR_NONE;

    return HWC2_ERROR_NO_RESOURCES;
}

hwc2_error_t hwc2_display::decompress_window_buffers()
{
    hwc2_error_t ret;

    for (auto &win: windows) {
        if (win.contains_client_target()) {
            ret = client_target.decompress();
            if (ret != HWC2_ERROR_NONE) {
                ALOGE("dpy %" PRIu64 ": failed to decompress client target"
                        " buffer", id);
                return ret;
            }
        } else if (win.contains_layer()) {
            ret = layers.at(win.get_layer()).decompress_buffer();
            if (ret != HWC2_ERROR_NONE) {
                ALOGE("dpy %" PRIu64 " lyr %" PRIu64 ": failed to decompress"
                        " layer buffer", id, win.get_layer());
                return ret;
            }
        }
    }
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
    set_client_target_properties();

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::get_color_modes(uint32_t *out_num_modes,
        android_color_mode_t *out_modes) const
{
    *out_num_modes = 1;
    if (out_modes)
        *out_modes = HAL_COLOR_MODE_NATIVE;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::set_color_mode(android_color_mode_t mode)
{
    switch (mode) {
    case HAL_COLOR_MODE_NATIVE:
        return HWC2_ERROR_NONE;
    case HAL_COLOR_MODE_STANDARD_BT601_625:
    case HAL_COLOR_MODE_STANDARD_BT601_625_UNADJUSTED:
    case HAL_COLOR_MODE_STANDARD_BT601_525:
    case HAL_COLOR_MODE_STANDARD_BT601_525_UNADJUSTED:
    case HAL_COLOR_MODE_STANDARD_BT709:
    case HAL_COLOR_MODE_DCI_P3:
    case HAL_COLOR_MODE_SRGB:
    case HAL_COLOR_MODE_ADOBE_RGB:
        ALOGE("dpy %" PRIu64 ": unsupported color mode", id);
        return HWC2_ERROR_UNSUPPORTED;
    default:
        ALOGE("dpy %" PRIu64 ": invalid color mode", id);
        return HWC2_ERROR_BAD_PARAMETER;
    }

}

hwc2_error_t hwc2_display::get_hdr_capabilities(uint32_t *out_num_types,
        android_hdr_t* /*out_types*/, float* /*out_max_luminance*/,
        float* /*out_max_average_luminance*/,
        float* /*out_min_luminance*/) const
{
    *out_num_types = 0;
    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::set_color_transform(const float *color_matrix,
        android_color_transform_t color_hint)
{
    switch (color_hint) {
    case HAL_COLOR_TRANSFORM_IDENTITY:
    case HAL_COLOR_TRANSFORM_ARBITRARY_MATRIX:
    case HAL_COLOR_TRANSFORM_VALUE_INVERSE:
    case HAL_COLOR_TRANSFORM_GRAYSCALE:
    case HAL_COLOR_TRANSFORM_CORRECT_PROTANOPIA:
    case HAL_COLOR_TRANSFORM_CORRECT_DEUTERANOPIA:
    case HAL_COLOR_TRANSFORM_CORRECT_TRITANOPIA:
        memcpy(this->color_matrix.data(), color_matrix,
                this->color_matrix.size());
        this->color_hint = color_hint;
        return HWC2_ERROR_NONE;
    default:
        ALOGE("dpy %" PRIu64 ": invalid color transform hint", id);
        return HWC2_ERROR_BAD_PARAMETER;
    }
}

hwc2_error_t hwc2_display::get_client_target_support(uint32_t width,
        uint32_t height, android_pixel_format_t format,
        android_dataspace_t dataspace)
{
    if (active_config >= configs.size()) {
        ALOGE("dpy %" PRIu64 ": no active_config", id);
        return HWC2_ERROR_UNSUPPORTED;
    }

    int32_t cnfg_width = configs[active_config].get_attribute(
            HWC2_ATTRIBUTE_WIDTH);
    int32_t cnfg_height = configs[active_config].get_attribute(
            HWC2_ATTRIBUTE_HEIGHT);

    if (cnfg_width < 0 || width != static_cast<uint32_t>(cnfg_width)) {
        ALOGE("dpy %" PRIu64 ": unsupported client target width", id);
        return HWC2_ERROR_UNSUPPORTED;
    }

    if (cnfg_height < 0 || height != static_cast<uint32_t>(cnfg_height)) {
        ALOGE("dpy %" PRIu64 ": unsupported client target height", id);
        return HWC2_ERROR_UNSUPPORTED;
    }

    if (format != HAL_PIXEL_FORMAT_RGBA_8888) {
        ALOGE("dpy %" PRIu64 ": unsupported client target format", id);
        return HWC2_ERROR_UNSUPPORTED;
    }

    if (dataspace != HAL_DATASPACE_UNKNOWN) {
        ALOGE("dpy %" PRIu64 ": unsupported client target dataspace", id);
        return HWC2_ERROR_UNSUPPORTED;
    }

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::set_client_target(buffer_handle_t handle,
        int32_t acquire_fence, android_dataspace_t dataspace,
        const hwc_region_t &surface_damage)
{
    hwc2_error_t ret = client_target.set_buffer(handle, acquire_fence);
    if (ret != HWC2_ERROR_NONE)
        return ret;

    ret = client_target.set_dataspace(dataspace);
    if (ret != HWC2_ERROR_NONE)
        return ret;

    return client_target.set_surface_damage(surface_damage);
}

hwc2_error_t hwc2_display::set_client_target_properties()
{
    int32_t width = configs.at(active_config).get_attribute(HWC2_ATTRIBUTE_WIDTH);
    int32_t height = configs.at(active_config).get_attribute(HWC2_ATTRIBUTE_HEIGHT);

    hwc_rect_t frame;
    frame.left = 0;
    frame.top = 0;
    frame.right = width;
    frame.bottom = height;
    client_target.set_display_frame(frame);

    hwc_frect_t crop;
    crop.left = 0.0;
    crop.top = 0.0;
    crop.right = static_cast<float>(width);
    crop.bottom = static_cast<float>(height);
    client_target.set_source_crop(crop);

    client_target.set_blend_mode(HWC2_BLEND_MODE_PREMULTIPLIED);
    client_target.set_z_order(UINT32_MAX);

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::create_layer(hwc2_layer_t *out_layer)
{
    display_state = modified;

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

    display_state = modified;
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

    hwc2_error_t ret = it->second.set_comp_type(comp_type);

    if (it->second.get_modified())
        display_state = modified;

    return ret;
}

hwc2_error_t hwc2_display::set_layer_buffer(hwc2_layer_t lyr_id,
        buffer_handle_t handle, int32_t acquire_fence)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    hwc2_error_t ret = it->second.set_buffer(handle, acquire_fence);

    if (it->second.get_modified())
        display_state = modified;

    return ret;
}

hwc2_error_t hwc2_display::set_layer_dataspace(hwc2_layer_t lyr_id,
        android_dataspace_t dataspace)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    hwc2_error_t ret = it->second.set_dataspace(dataspace);

    if (it->second.get_modified())
        display_state = modified;

    return ret;
}

hwc2_error_t hwc2_display::set_layer_display_frame(hwc2_layer_t lyr_id,
        const hwc_rect_t &display_frame)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    hwc2_error_t ret = it->second.set_display_frame(display_frame);

    if (it->second.get_modified())
        display_state = modified;

    return ret;
}

hwc2_error_t hwc2_display::set_layer_source_crop(hwc2_layer_t lyr_id,
        const hwc_frect_t &source_crop)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    hwc2_error_t ret = it->second.set_source_crop(source_crop);

    if (it->second.get_modified())
        display_state = modified;

    return ret;
}

hwc2_error_t hwc2_display::set_layer_z_order(hwc2_layer_t lyr_id, uint32_t z_order)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    hwc2_error_t ret = it->second.set_z_order(z_order);

    if (it->second.get_modified())
        display_state = modified;

    return ret;
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

    hwc2_error_t ret = it->second.set_blend_mode(blend_mode);

    if (it->second.get_modified())
        display_state = modified;

    return ret;
}

hwc2_error_t hwc2_display::set_layer_plane_alpha(hwc2_layer_t lyr_id, float plane_alpha)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    hwc2_error_t ret = it->second.set_plane_alpha(plane_alpha);

    if (it->second.get_modified())
        display_state = modified;

    return ret;
}

hwc2_error_t hwc2_display::set_layer_transform(hwc2_layer_t lyr_id,
        const hwc_transform_t transform)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    hwc2_error_t ret = it->second.set_transform(transform);

    if (it->second.get_modified())
        display_state = modified;

    return ret;
}

hwc2_error_t hwc2_display::set_layer_visible_region(hwc2_layer_t lyr_id,
        const hwc_region_t &visible_region)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    hwc2_error_t ret = it->second.set_visible_region(visible_region);

    if (it->second.get_modified())
        display_state = modified;

    return ret;
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
