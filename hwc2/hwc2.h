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
#include <array>
#include <mutex>
#include <string>

#include <adf/adf.h>
#include <adfhwc/adfhwc.h>

#define HWC2_WINDOW_COUNT         4

#define HWC2_WINDOW_MIN_SOURCE_CROP_WIDTH       1.0
#define HWC2_WINDOW_MIN_SOURCE_CROP_HEIGHT      1.0
#define HWC2_WINDOW_MIN_DISPLAY_FRAME_WIDTH     4
#define HWC2_WINDOW_MIN_DISPLAY_FRAME_HEIGHT    4
#define HWC2_WINDOW_MIN_DISPLAY_FRAME_SCALE     0.5

#define HWC2_WINDOW_MAX_ROT_SRC_HEIGHT            2560
#define HWC2_WINDOW_MAX_ROT_SRC_HEIGHT_NO_SCALE   3600

#define HWC2_WINDOW_CAP_YUV                  0x001
#define HWC2_WINDOW_CAP_SCALE                0x002
#define HWC2_WINDOW_CAP_FLIP                 0x004
#define HWC2_WINDOW_CAP_ROTATE_PACKED        0x008
#define HWC2_WINDOW_CAP_ROTATE_PLANAR        0x010
#define HWC2_WINDOW_CAP_PITCH                0x020
#define HWC2_WINDOW_CAP_TILED                0x040
#define HWC2_WINDOW_CAP_BLOCK_LINEAR         0x080

#define HWC2_WINDOW_CAP_LAYOUTS (HWC2_WINDOW_CAP_PITCH \
                               | HWC2_WINDOW_CAP_TILED \
                               | HWC2_WINDOW_CAP_BLOCK_LINEAR)

#define HWC2_WINDOW_CAP_COMMON (HWC2_WINDOW_CAP_YUV   \
                               | HWC2_WINDOW_CAP_SCALE \
                               | HWC2_WINDOW_CAP_FLIP  \
                               | HWC2_WINDOW_CAP_ROTATE_PACKED \
                               | HWC2_WINDOW_CAP_PITCH \
                               | HWC2_WINDOW_CAP_TILED \
                               | HWC2_WINDOW_CAP_BLOCK_LINEAR)

static const std::array<uint32_t, HWC2_WINDOW_COUNT> window_capabilities = {{
    HWC2_WINDOW_CAP_COMMON | HWC2_WINDOW_CAP_ROTATE_PLANAR,
    HWC2_WINDOW_CAP_COMMON,
    HWC2_WINDOW_CAP_COMMON,
    HWC2_WINDOW_CAP_PITCH
}};

class hwc2_gralloc {
public:
    /* hwc2_gralloc follows the singleton design pattern */
    static const hwc2_gralloc &get_instance();

    bool     is_valid(buffer_handle_t handle) const;
    bool     is_stereo(buffer_handle_t handle) const;
    bool     is_yuv(buffer_handle_t handle) const;
    int      get_format(buffer_handle_t handle) const;
    void     get_surfaces(buffer_handle_t handle, const void **surf,
                 size_t *surf_cnt) const;
    int32_t  get_layout(const void *surf, uint32_t surf_idx) const;

private:
    hwc2_gralloc();
    ~hwc2_gralloc();

    /* The address of the nvgr_is_valid symbol. This NVIDIA function checks if a
     * buffer is valid */
    bool (*nvgr_is_valid)(buffer_handle_t handle);

    /* The address of the nvgr_is_stereo symbol. This NVIDIA function checks if
     * a buffer is stereo */
    bool (*nvgr_is_stereo)(buffer_handle_t handle);

    /* The address of the nvgr_is_yuv symbol. This NVIDIA function checks if a
     * buffer is yuv */
    bool (*nvgr_is_yuv)(buffer_handle_t handle);

    /* The address of the nvgr_get_format symbol. This NVIDIA function returns
     * the format of a buffer */
    int (*nvgr_get_format)(buffer_handle_t handle);

    /* The address of the nvgr_get_surfaces symbol. This NVIDIA function returns
     * the surfaces associated with a buffer handle */
    void (*nvgr_get_surfaces)(buffer_handle_t handle, const void **surf,
            size_t *surf_cnt);

    /* A symbol table handle to the NVIDIA gralloc .so file. */
    void *nvgr;
};

class hwc2_buffer {
public:
    hwc2_buffer();
    ~hwc2_buffer();

    void close_acquire_fence();

    /* Get properties */
    buffer_handle_t  get_buffer_handle() const { return handle; }
    hwc_transform_t  get_transform() const { return transform; }
    uint32_t         get_adf_buffer_format() const;
    uint32_t         get_layout() const;
    int     get_display_frame_width() const;
    int     get_display_frame_height() const;
    float   get_source_crop_width() const;
    float   get_source_crop_height() const;
    float   get_scale_width() const;
    float   get_scale_height() const;
    void    get_surfaces(const void **surf, size_t *surf_cnt) const;
    bool    is_source_crop_int_aligned() const;
    bool    is_stereo() const;
    bool    is_yuv() const;

    /* Set properties */
    hwc2_error_t set_buffer(buffer_handle_t handle, int32_t acquire_fence);
    hwc2_error_t set_dataspace(android_dataspace_t dataspace);
    hwc2_error_t set_display_frame(const hwc_rect_t &display_frame);
    hwc2_error_t set_source_crop(const hwc_frect_t &source_crop);
    hwc2_error_t set_z_order(uint32_t z_order);
    hwc2_error_t set_surface_damage(const hwc_region_t &surface_damage);
    hwc2_error_t set_blend_mode(hwc2_blend_mode_t blend_mode);
    hwc2_error_t set_plane_alpha(float plane_alpha);
    hwc2_error_t set_transform(hwc_transform_t transform);
    hwc2_error_t set_visible_region(const hwc_region_t &visible_region);

private:
    /* A handle to the buffer */
    buffer_handle_t handle;

    /* A sync fence object which will be signaled when it is safe to read
     * from the buffer. If the acquire_fence is -1, it is already safe to
     * read from the buffer */
    int32_t acquire_fence;

    /* Provides more info on how to interpret the buffer contents such as
     * the encoding standard and color transformation */
    android_dataspace_t dataspace;

    /* The portion of the display covered by the buffer */
    hwc_rect_t display_frame;

    /* The portion of the source buffer which will fill the display frame */
    hwc_frect_t source_crop;

    /* The height of a given buffer. A buffer with a greater Z value occludes
     * a buffer with a lesser Z value */
    uint32_t z_order;

    /* The region of the source buffer which has been modified since the
     * last frame */
    std::vector<hwc_rect_t> surface_damage;

    /* The blend mode of the buffer */
    hwc2_blend_mode_t blend_mode;

    /* An alpha value in the range [0.0, 1.0] to be applied to the whole
     * buffer */
    float plane_alpha;

    /* The rotation or flip of the buffer */
    hwc_transform_t transform;

    /* The portion of the layer that is visible including portions under
     * translucent areas of other buffers */
    std::vector<hwc_rect_t> visible_region;
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
    buffer_handle_t     get_buffer_handle() const;
    hwc_transform_t     get_transform() const;
    uint32_t            get_adf_buffer_format() const;
    uint32_t            get_layout() const;
    int     get_display_frame_width() const;
    int     get_display_frame_height() const;
    float   get_source_crop_width() const;
    float   get_source_crop_height() const;
    float   get_scale_width() const;
    float   get_scale_height() const;
    void    get_surfaces(const void **surf, size_t *surf_cnt) const;
    bool    is_source_crop_int_aligned() const;
    bool    is_stereo() const;
    bool    is_yuv() const;

    /* Set properties */
    hwc2_error_t set_comp_type(hwc2_composition_t comp_type);
    hwc2_error_t set_buffer(buffer_handle_t handle, int32_t acquire_fence);
    hwc2_error_t set_dataspace(android_dataspace_t dataspace);
    hwc2_error_t set_display_frame(const hwc_rect_t &display_frame);
    hwc2_error_t set_source_crop(const hwc_frect_t &source_crop);
    hwc2_error_t set_z_order(uint32_t z_order);
    hwc2_error_t set_surface_damage(const hwc_region_t &surface_damage);
    hwc2_error_t set_blend_mode(hwc2_blend_mode_t blend_mode);
    hwc2_error_t set_plane_alpha(float plane_alpha);
    hwc2_error_t set_transform(hwc_transform_t transform);
    hwc2_error_t set_visible_region(const hwc_region_t &visible_region);

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

class hwc2_window {
public:
    hwc2_window();

    void clear();
    hwc2_error_t assign_client_target(uint32_t z_order);
    hwc2_error_t assign_layer(uint32_t z_order, const hwc2_layer &lyr);

    bool is_empty() const;
    bool contains_client_target() const;
    bool contains_layer() const;

    uint32_t     get_z_order() const { return z_order; }
    hwc2_layer_t get_layer() const { return lyr_id; }

    bool has_layer_requirements(const hwc2_layer &lyr) const;
    bool has_requirements(uint32_t required_capabilities) const;
    void set_capabilities(uint32_t capabilities);

    static bool is_supported(const hwc2_layer &lyr);

private:
    /* Each window can contain the client target, a layer or nothing */
    enum hwc2_window_content_t {
        HWC2_WINDOW_CONTENT_EMPTY,
        HWC2_WINDOW_CONTENT_CLIENT_TARGET,
        HWC2_WINDOW_CONTENT_LAYER
    } content;

    /* If the content is not HWC2_WINDOW_EMPTY, the z_order corresponds to the
     * display controller z order. The display controller z order is the reverse
     * of the SurfaceFlinger z order. In the display controller z order, a
     * window with lower z order occludes a window with higher z order */
    uint32_t z_order;

    /* If the content is HWC2_WINDOW_LAYER, lyr_id is the layer it contains */
    hwc2_layer_t lyr_id;

    /* The capabilities the underlying hardware window can support such as
     * rotation, scaling, flipping, yuv, and surface layouts */
    uint32_t capabilities;
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

    /* Window functions */
    void init_windows();
    void clear_windows();
    hwc2_error_t assign_client_target_window(uint32_t z_order);
    hwc2_error_t assign_layer_window(uint32_t z_order, hwc2_layer_t lyr_id);

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
    hwc2_error_t set_layer_buffer(hwc2_layer_t lyr_id, buffer_handle_t handle,
                    int32_t acquire_fence);
    hwc2_error_t set_layer_dataspace(hwc2_layer_t lyr_id,
                    android_dataspace_t dataspace);
    hwc2_error_t set_layer_display_frame(hwc2_layer_t lyr_id,
                    const hwc_rect_t &display_frame);
    hwc2_error_t set_layer_source_crop(hwc2_layer_t lyr_id,
                    const hwc_frect_t &source_crop);
    hwc2_error_t set_layer_z_order(hwc2_layer_t lyr_id, uint32_t z_order);
    hwc2_error_t set_layer_surface_damage(hwc2_layer_t lyr_id,
                    const hwc_region_t &surface_damage);
    hwc2_error_t set_layer_blend_mode(hwc2_layer_t lyr_id,
                    hwc2_blend_mode_t blend_mode);
    hwc2_error_t set_layer_plane_alpha(hwc2_layer_t lyr_id, float plane_alpha);
    hwc2_error_t set_layer_transform(hwc2_layer_t lyr_id,
                    hwc_transform_t transform);
    hwc2_error_t set_layer_visible_region(hwc2_layer_t lyr_id,
                    const hwc_region_t &visible_region);
    hwc2_error_t set_layer_color(hwc2_layer_t lyr_id, const hwc_color_t &color);
    hwc2_error_t set_cursor_position(hwc2_layer_t lyr_id, int32_t x, int32_t y);

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

    /* The display windows */
    std::array<hwc2_window, HWC2_WINDOW_COUNT> windows;

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
    hwc2_error_t set_layer_buffer(hwc2_display_t dpy_id, hwc2_layer_t lyr_id,
                    buffer_handle_t handle, int32_t acquire_fence);
    hwc2_error_t set_layer_dataspace(hwc2_display_t dpy_id, hwc2_layer_t lyr_id,
                    android_dataspace_t dataspace);
    hwc2_error_t set_layer_display_frame(hwc2_display_t dpy_id,
                    hwc2_layer_t lyr_id, const hwc_rect_t &display_frame);
    hwc2_error_t set_layer_source_crop(hwc2_display_t dpy_id,
                    hwc2_layer_t lyr_id, const hwc_frect_t &source_crop);
    hwc2_error_t set_layer_z_order(hwc2_display_t dpy_id, hwc2_layer_t lyr_id,
                    uint32_t z_order);
    hwc2_error_t set_layer_surface_damage(hwc2_display_t dpy_id,
                    hwc2_layer_t lyr_id, const hwc_region_t &surface_damage);
    hwc2_error_t set_layer_blend_mode(hwc2_display_t dpy_id,
                    hwc2_layer_t lyr_id, hwc2_blend_mode_t blend_mode);
    hwc2_error_t set_layer_plane_alpha(hwc2_display_t dpy_id,
                    hwc2_layer_t lyr_id, float plane_alpha);
    hwc2_error_t set_layer_transform(hwc2_display_t dpy_id, hwc2_layer_t lyr_id,
                    hwc_transform_t transform);
    hwc2_error_t set_layer_visible_region(hwc2_display_t dpy_id,
                    hwc2_layer_t lyr_id, const hwc_region_t &visible_region);
    hwc2_error_t set_layer_color(hwc2_display_t dpy_id, hwc2_layer_t lyr_id,
                    const hwc_color_t &color);
    hwc2_error_t set_cursor_position(hwc2_display_t dpy_id, hwc2_layer_t lyr_id,
                    int32_t x, int32_t y);

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
