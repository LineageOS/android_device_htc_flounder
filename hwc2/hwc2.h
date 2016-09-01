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

#include <adf/adf.h>
#include <adfhwc/adfhwc.h>

class hwc2_config {
public:
    hwc2_config();

    int set_attribute(hwc2_attribute_t attribute, int32_t value);

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

private:
    /* All of the client callback functions and their data */
    hwc2_callback_data_t hotplug_data;
    HWC2_PFN_HOTPLUG hotplug;

    hwc2_callback_data_t refresh_data;
    HWC2_PFN_REFRESH refresh;

    hwc2_callback_data_t vsync_data;
    HWC2_PFN_VSYNC vsync;
};

class hwc2_display {
public:
    hwc2_display(hwc2_display_t id, int adf_intf_fd,
                const struct adf_device &adf_dev);
    ~hwc2_display();

    hwc2_display_t get_id() const { return id; }

    int retrieve_display_configs(struct adf_hwc_helper *adf_helper);

    static hwc2_display_t get_next_id();

    static void reset_ids() { display_cnt = 0; }

private:
    /* Identifies the display to the client */
    hwc2_display_t id;

    /* All the valid configurations for the display */
    std::unordered_map<hwc2_config_t, hwc2_config> configs;

    /* The id of the current active configuration of the display */
    hwc2_config_t active_config;

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
