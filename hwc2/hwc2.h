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

#ifndef _HWC2_H
#define _HWC2_H

#include <hardware/hwcomposer2.h>

#include <unordered_map>
#include <queue>
#include <mutex>
#include <string>

#include <adf/adf.h>
#include <adfhwc/adfhwc.h>

class hwc2_buffer {
public:
    hwc2_buffer();

    /* Set properties */
    hwc2_error_t set_dataspace(android_dataspace_t dataspace);
    hwc2_error_t set_blend_mode(hwc2_blend_mode_t blend_mode);
    hwc2_error_t set_plane_alpha(float plane_alpha);

private:
    /* Provides more info on how to interpret the buffer contents such as
     * the encoding standard and color transformation */
    android_dataspace_t dataspace;

    /* The blend mode of the buffer */
    hwc2_blend_mode_t blend_mode;

    /* An alpha value in the range [0.0, 1.0] to be applied to the whole
     * buffer */
    float plane_alpha;
};

class hwc2_config {
public:
    hwc2_config();

    int set_attribute(hwc2_attribute_t attribute, int32_t value);
    int32_t get_attribute(hwc2_attribute_t attribute) const;

private:
    /* Dimensions in pixels */
    int32_t width;
    int32_t height;

    /* Vsync period in nanoseconds */
    int32_t vsync_period;

    /* Dots per thousand inches (DPI * 1000) */
    int32_t dpi_x;
    int32_t dpi_y;
};

class hwc2_callback {
public:
    hwc2_callback();

    hwc2_error_t register_callback(hwc2_callback_descriptor_t descriptor,
            hwc2_callback_data_t callback_data,
            hwc2_function_pointer_t pointer);

    void call_hotplug(hwc2_display_t dpy_id, hwc2_connection_t connection);
    void call_vsync(hwc2_display_t dpy_id, int64_t timestamp);

private:
    std::mutex state_mutex;

    /* A queue of all the hotplug notifications that were called before the
     * callback was registered */
    std::queue<std::pair<hwc2_display_t, hwc2_connection_t>> hotplug_pending;

    /* All of the client callback functions and their data */
    hwc2_callback_data_t hotplug_data;
    HWC2_PFN_HOTPLUG hotplug;

    hwc2_callback_data_t refresh_data;
    HWC2_PFN_REFRESH refresh;

    hwc2_callback_data_t vsync_data;
    HWC2_PFN_VSYNC vsync;
};

class hwc2_layer {
public:
    hwc2_layer(hwc2_layer_t id);

    /* Get properties */
    hwc2_layer_t        get_id() const { return id; }
    hwc2_composition_t  get_comp_type() const { return comp_type; }

    /* Set properties */
    hwc2_error_t set_comp_type(hwc2_composition_t comp_type);
    hwc2_error_t set_dataspace(android_dataspace_t dataspace);
    hwc2_error_t set_blend_mode(hwc2_blend_mode_t blend_mode);
    hwc2_error_t set_plane_alpha(float plane_alpha);

    static hwc2_layer_t get_next_id();

private:
    /* Identifies the layer to the client */
    hwc2_layer_t id;

    /* The buffer containing the data to be displayed */
    hwc2_buffer buffer;

    /* Composition type of the layer */
    hwc2_composition_t comp_type;

    /* Keep track to total number of layers so new layer ids can be
     * generated */
    static uint64_t layer_cnt;
};

class hwc2_display {
public:
    hwc2_display(hwc2_display_t id, int adf_intf_fd,
                const struct adf_device &adf_dev, hwc2_connection_t connection,
                hwc2_display_type_t type, hwc2_power_mode_t power_mode);
    ~hwc2_display();

    /* Display functions */
    hwc2_display_t      get_id() const { return id; }
    hwc2_display_type_t get_type() const { return type; }
    hwc2_connection_t   get_connection() const { return connection; }
    hwc2_vsync_t        get_vsync_enabled() const { return vsync_enabled; }
    hwc2_error_t        get_name(uint32_t *out_size, char *out_name) const;
    void                init_name();

    hwc2_error_t set_connection(hwc2_connection_t connection);
    hwc2_error_t set_vsync_enabled(hwc2_vsync_t enabled);

    /* Power modes */
    hwc2_error_t set_power_mode(hwc2_power_mode_t mode);
    hwc2_error_t get_doze_support(int32_t *out_support) const;

    /* Config functions */
    int          retrieve_display_configs(struct adf_hwc_helper *adf_helper);
    hwc2_error_t get_display_attribute(hwc2_config_t config,
                    hwc2_attribute_t attribute, int32_t *out_value) const;
    hwc2_error_t get_display_configs(uint32_t *out_num_configs,
                    hwc2_config_t *out_configs) const;
    hwc2_error_t get_active_config(hwc2_config_t *out_config) const;
    hwc2_error_t set_active_config(struct adf_hwc_helper *adf_helper,
                    hwc2_config_t config);

    /* Set layer functions */
    hwc2_error_t create_layer(hwc2_layer_t *out_layer);
    hwc2_error_t destroy_layer(hwc2_layer_t lyr_id);

    hwc2_error_t set_layer_composition_type(hwc2_layer_t lyr_id,
                    hwc2_composition_t comp_type);
    hwc2_error_t set_layer_dataspace(hwc2_layer_t lyr_id,
                    android_dataspace_t dataspace);
    hwc2_error_t set_layer_blend_mode(hwc2_layer_t lyr_id,
                    hwc2_blend_mode_t blend_mode);
    hwc2_error_t set_layer_plane_alpha(hwc2_layer_t lyr_id, float plane_alpha);

    static hwc2_display_t get_next_id();

    static void reset_ids() { display_cnt = 0; }

private:
    /* Identifies the display to the client */
    hwc2_display_t id;

    /* A human readable version of the display's name */
    std::string name;

    /* The display is connected to an output */
    hwc2_connection_t connection;

    /* Physical or virtual */
    hwc2_display_type_t type;

    /* The layers currently in use */
    std::unordered_map<hwc2_layer_t, hwc2_layer> layers;

    /* Is vsync enabled */
    hwc2_vsync_t vsync_enabled;

    /* All the valid configurations for the display */
    std::unordered_map<hwc2_config_t, hwc2_config> configs;

    /* The id of the current active configuration of the display */
    hwc2_config_t active_config;

    /* The current power mode of the display */
    hwc2_power_mode_t power_mode;

    /* The adf interface file descriptor for the display */
    int adf_intf_fd;

    /* The adf device associated with the display */
    struct adf_device adf_dev;

    /* Keep track to total number of displays so new display ids can be
     * generated */
    static uint64_t display_cnt;
};

class hwc2_dev {
public:
    hwc2_dev();
    ~hwc2_dev();

    /* Display functions */
    hwc2_error_t get_display_name(hwc2_display_t dpy_id, uint32_t *out_size,
                    char *out_name) const;
    hwc2_error_t get_display_type(hwc2_display_t dpy_id,
                    hwc2_display_type_t *out_type) const;

    /* Power modes */
    hwc2_error_t set_power_mode(hwc2_display_t dpy_id, hwc2_power_mode_t mode);
    hwc2_error_t get_doze_support(hwc2_display_t dpy_id, int32_t *out_support)
                    const;

    /* Config functions */
    hwc2_error_t get_display_attribute(hwc2_display_t dpy_id,
                    hwc2_config_t config, hwc2_attribute_t attribute,
                    int32_t *out_value) const;
    hwc2_error_t get_display_configs(hwc2_display_t dpy_id,
                    uint32_t *out_num_configs, hwc2_config_t *out_configs) const;
    hwc2_error_t get_active_config(hwc2_display_t dpy_id,
                    hwc2_config_t *out_config) const;
    hwc2_error_t set_active_config(hwc2_display_t dpy_id, hwc2_config_t config);

    /* Layer functions */
    hwc2_error_t create_layer(hwc2_display_t dpy_id, hwc2_layer_t *out_layer);
    hwc2_error_t destroy_layer(hwc2_display_t dpy_id, hwc2_layer_t lyr_id);

    hwc2_error_t set_layer_composition_type(hwc2_display_t dpy_id,
                    hwc2_layer_t lyr_id, hwc2_composition_t comp_type);
    hwc2_error_t set_layer_dataspace(hwc2_display_t dpy_id, hwc2_layer_t lyr_id,
                    android_dataspace_t dataspace);
    hwc2_error_t set_layer_blend_mode(hwc2_display_t dpy_id,
                    hwc2_layer_t lyr_id, hwc2_blend_mode_t blend_mode);
    hwc2_error_t set_layer_plane_alpha(hwc2_display_t dpy_id,
                    hwc2_layer_t lyr_id, float plane_alpha);

    /* Callback functions */
    void hotplug(hwc2_display_t dpy_id, hwc2_connection_t connection);
    void vsync(hwc2_display_t dpy_id, uint64_t timestamp);

    hwc2_error_t set_vsync_enabled(hwc2_display_t dpy_id, hwc2_vsync_t enabled);

    hwc2_error_t register_callback(hwc2_callback_descriptor_t descriptor,
                    hwc2_callback_data_t callback_data,
                    hwc2_function_pointer_t pointer);

    int open_adf_device();

private:
    /* General callback functions for all displays */
    hwc2_callback callback_handler;

    /* The physical and virtual displays associated with this device */
    std::unordered_map<hwc2_display_t, hwc2_display> displays;

    /* The associated adf hardware composer helper */
    struct adf_hwc_helper *adf_helper;

    int open_adf_display(adf_id_t adf_id);
};

struct hwc2_context {
    hwc2_device_t hwc2_device; /* must be first member in struct */

    hwc2_dev *hwc2_dev;
};

#endif /* ifndef _HWC2_H */
