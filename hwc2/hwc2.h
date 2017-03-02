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

#define HWC2_INCLUDE_STRINGIFICATION
#define HWC2_USE_CPP11
#include <hardware/hwcomposer2.h>
#undef HWC2_INCLUDE_STRINGIFICATION
#undef HWC2_USE_CPP11

#include <android-base/unique_fd.h>

#include <unordered_map>
#include <queue>
#include <array>
#include <vector>
#include <string>
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
    void     get_dma_buf(const void *surf, uint32_t surf_idx, int *out_fd) const;
    int32_t  get_layout(const void *surf, uint32_t surf_idx) const;
    uint32_t get_pitch(const void *surf, uint32_t surf_idx) const;
    uint32_t get_hmem(const void *surf, uint32_t surf_idx) const;
    uint32_t get_offset(const void *surf, uint32_t surf_idx) const;
    uint32_t get_block_height_log2(const void *surf, uint32_t surf_idx) const;
    uint32_t decompress(buffer_handle_t handle, int in_fence,
                 int *out_fence) const;

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

    /* The address of the NvRmMemDmaBufFdFromHandle symbol. This NVIDIA function
     * returns the dma bufs assicated with a buffer handle */
    void (*NvRmMemDmaBufFdFromHandle)(uint32_t hmem, int *fd);

    /* The address of the nvgr_decompress symbol. This NVIDIA function
     * decompresses the buffer and returns an out fence that fires when the
     * buffer is done decompressing */
     int (*nvgr_decompress)(buffer_handle_t handle, int in_fence,
            int *out_fence);

    /* A symbol table handle to the NVIDIA gralloc .so file. */
    void *nvgr;
};

class hwc2_buffer {
public:
    hwc2_buffer();
    ~hwc2_buffer();

    std::string dump() const;

    hwc2_error_t decompress();
    hwc2_error_t get_adf_post_props(struct tegra_adf_flip_windowattr *win_attr,
                    struct adf_buffer_config *adf_buf, size_t win_idx,
                    size_t buf_idx, uint32_t z_order) const;

    void close_acquire_fence();

    /* Get properties */
    uint32_t         get_z_order() const { return z_order; }
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
    bool    is_overlapped() const;

    bool    get_modified() const { return modified; }

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

    void set_modified(bool modified) { this->modified = modified; }

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

    /* Used to determine if the current buffer has a different format than the
     * previous buffer */
    uint32_t previous_format;

    /* The buffer is modified and will force revalidation of the display */
    bool modified;

    hwc2_error_t get_adf_buf_config(struct adf_buffer_config *adf_buf) const;
    hwc2_error_t get_adf_win_attr(struct tegra_adf_flip_windowattr *win_attr,
                        size_t win_idx, size_t buf_idx, uint32_t z_order) const;
};

class hwc2_config {
public:
    hwc2_config();

    std::string dump() const;

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

    std::string dump() const;

    hwc2_error_t decompress_buffer();

    hwc2_error_t get_adf_post_props(struct tegra_adf_flip_windowattr *win_attr,
                    struct adf_buffer_config *adf_buf, size_t win_idx,
                    size_t buf_idx, uint32_t z_order) const;

    void close_acquire_fence() { buffer.close_acquire_fence(); }

    /* Get properties */
    hwc2_layer_t        get_id() const { return id; }
    hwc2_composition_t  get_comp_type() const { return comp_type; }
    uint32_t            get_z_order() const { return buffer.get_z_order(); }
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
    bool    is_overlapped() const;

    bool    get_modified() const { return modified || buffer.get_modified(); }

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

    void set_modified(bool modified) { this->modified = modified;
                        buffer.set_modified(modified); }

    static hwc2_layer_t get_next_id();

private:
    /* Identifies the layer to the client */
    hwc2_layer_t id;

    /* The buffer containing the data to be displayed */
    hwc2_buffer buffer;

    /* Composition type of the layer */
    hwc2_composition_t comp_type;

    /* The layer is modified and will force revalidation of the display */
    bool modified;

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

    std::string dump() const;

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

    /* Display present functions */
    hwc2_error_t validate_display(uint32_t *out_num_types,
                    uint32_t *out_num_requests);
    void         force_client_composition();
    void         assign_composition();

    hwc2_error_t get_changed_composition_types(uint32_t *out_num_elements,
                    hwc2_layer_t *out_layers, hwc2_composition_t *out_types)
                    const;
    hwc2_error_t get_display_requests(
                    hwc2_display_request_t *out_display_requests,
                    uint32_t *out_num_elements, hwc2_layer_t *out_layers,
                    hwc2_layer_request_t *out_layer_requests) const;
    hwc2_error_t accept_display_changes();

    hwc2_error_t present_display(int32_t *out_present_fence);
    hwc2_error_t prepare_present_display();
    void         close_acquire_fences();

    hwc2_error_t get_release_fences(uint32_t *out_num_elements,
                    hwc2_layer_t *out_layers, int32_t *out_fences) const;

    /* Window functions */
    void init_windows();
    void clear_windows();
    hwc2_error_t assign_client_target_window(uint32_t z_order);
    hwc2_error_t assign_layer_window(uint32_t z_order, hwc2_layer_t lyr_id);

    hwc2_error_t  decompress_window_buffers();

    /* Config functions */
    int          retrieve_display_configs(struct adf_hwc_helper *adf_helper);
    hwc2_error_t get_display_attribute(hwc2_config_t config,
                    hwc2_attribute_t attribute, int32_t *out_value) const;
    hwc2_error_t get_display_configs(uint32_t *out_num_configs,
                    hwc2_config_t *out_configs) const;
    hwc2_error_t get_active_config(hwc2_config_t *out_config) const;
    hwc2_error_t set_active_config(struct adf_hwc_helper *adf_helper,
                    hwc2_config_t config);

    /* Color/hdr functions */
    hwc2_error_t get_color_modes(uint32_t *out_num_modes,
                    android_color_mode_t *out_modes) const;
    hwc2_error_t set_color_mode(android_color_mode_t mode);
    hwc2_error_t get_hdr_capabilities(uint32_t *out_num_types,
                    android_hdr_t *out_types, float *out_max_luminance,
                    float *out_max_average_luminance,
                    float *out_min_luminance) const;
    hwc2_error_t set_color_transform(const float *color_matrix,
                    android_color_transform_t color_hint);

    /* Client target functions */
    hwc2_error_t get_client_target_support(uint32_t width, uint32_t height,
                    android_pixel_format_t format,
                    android_dataspace_t dataspace);
    hwc2_error_t set_client_target(buffer_handle_t target,
                    int32_t acquire_fence, android_dataspace_t dataspace,
                    const hwc_region_t &surface_damage);
    hwc2_error_t set_client_target_properties();

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

    /* The current state of the display */
    enum display_state_t {
        /* The display has been modified since the last validate_display or
         * accept_display_changes call */
        modified = 0,
        /* The display failed the last validate_display attempt and has not been
         * modified since and has not called accept_display_changes*/
        invalid = 1,
        /* The display passed the last validate_display attempt or has called
         * accept_display_changes */
        valid = 2,
    } display_state;

    /* The last call to validate determined that the client target buffer is
     * necessary */
    bool client_target_used;

    /* The display windows */
    std::array<hwc2_window, HWC2_WINDOW_COUNT> windows;

    /* Recieves the output of the client composition */
    hwc2_buffer client_target;

    /* The layers currently in use */
    std::unordered_map<hwc2_layer_t, hwc2_layer> layers;

    /* Is vsync enabled */
    hwc2_vsync_t vsync_enabled;

    /* The layers that need a composition change. The list is populated during
     * validate_display. */
    std::unordered_map<hwc2_layer_t, hwc2_composition_t> changed_comp_types;

    /* All the valid configurations for the display */
    std::unordered_map<hwc2_config_t, hwc2_config> configs;

    /* The id of the current active configuration of the display */
    hwc2_config_t active_config;

    /* The current power mode of the display */
    hwc2_power_mode_t power_mode;

    /* Color transform which will be applied after composition */
    std::array<float, 16> color_matrix;

    /* A hit value which may be used instead of the given matrix unless it is
     * HAL_COLOR_TRANSFORM_ARBITRARY */
    android_color_transform_t color_hint;

    /* Sync fence object which will be signaled after the device has finished
     * reading from the buffer presented in the prior frame */
    android::base::unique_fd release_fence;

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

    std::string dump() const;
    void dump_hwc2(uint32_t *out_size, char *out_buffer);

    /* Display functions */
    hwc2_error_t get_display_name(hwc2_display_t dpy_id, uint32_t *out_size,
                    char *out_name);
    hwc2_error_t get_display_type(hwc2_display_t dpy_id,
                    hwc2_display_type_t *out_type);

    /* Virtual display functions */
    uint32_t     get_max_virtual_display_count();
    hwc2_error_t create_virtual_display(uint32_t width, uint32_t height,
                    android_pixel_format_t *format,
                    hwc2_display_t *out_display);
    hwc2_error_t destroy_virtual_display(hwc2_display_t dpy_id);
    hwc2_error_t set_output_buffer(hwc2_display_t dpy_id,
                    buffer_handle_t buffer, int32_t release_fence);

    /* Power modes */
    hwc2_error_t set_power_mode(hwc2_display_t dpy_id, hwc2_power_mode_t mode);
    hwc2_error_t get_doze_support(hwc2_display_t dpy_id, int32_t *out_support);

    /* Display present functions */
    hwc2_error_t validate_display(hwc2_display_t dpy_id,
                    uint32_t *out_num_types, uint32_t *out_num_requests);
    hwc2_error_t get_changed_composition_types(hwc2_display_t dpy_id,
                    uint32_t *out_num_elements, hwc2_layer_t *out_layers,
                    hwc2_composition_t *out_types);
    hwc2_error_t get_display_requests(hwc2_display_t dpy_id,
                    hwc2_display_request_t *out_display_requests,
                    uint32_t *out_num_elements, hwc2_layer_t *out_layers,
                    hwc2_layer_request_t *out_layer_requests);
    hwc2_error_t accept_display_changes(hwc2_display_t dpy_id);
    hwc2_error_t present_display(hwc2_display_t dpy_id,
                    int32_t *out_present_fence);
    hwc2_error_t get_release_fences(hwc2_display_t dpy_id,
                    uint32_t *out_num_elements, hwc2_layer_t *out_layers,
                    int32_t *out_fences);

    /* Config functions */
    hwc2_error_t get_display_attribute(hwc2_display_t dpy_id,
                    hwc2_config_t config, hwc2_attribute_t attribute,
                    int32_t *out_value);
    hwc2_error_t get_display_configs(hwc2_display_t dpy_id,
                    uint32_t *out_num_configs, hwc2_config_t *out_configs);
    hwc2_error_t get_active_config(hwc2_display_t dpy_id,
                    hwc2_config_t *out_config);
    hwc2_error_t set_active_config(hwc2_display_t dpy_id, hwc2_config_t config);

    /* Color/hdr functions */
    hwc2_error_t get_color_modes(hwc2_display_t dpy_id, uint32_t *out_num_modes,
                    android_color_mode_t *out_modes);
    hwc2_error_t set_color_mode(hwc2_display_t dpy_id,
                    android_color_mode_t mode);
    hwc2_error_t get_hdr_capabilities(hwc2_display_t dpy_id,
                    uint32_t *out_num_types, android_hdr_t *out_types,
                    float *out_max_luminance, float *out_max_average_luminance,
                    float *out_min_luminance);
    hwc2_error_t set_color_transform(hwc2_display_t dpy_id,
                    const float *color_matrix,
                    android_color_transform_t color_hint);

    /* Client target functions */
    hwc2_error_t get_client_target_support(hwc2_display_t dpy_id,
                    uint32_t width, uint32_t height,
                    android_pixel_format_t format,
                    android_dataspace_t dataspace);
    hwc2_error_t set_client_target(hwc2_display_t dpy_id,
                    buffer_handle_t target, int32_t acquire_fence,
                    android_dataspace_t dataspace,
                    const hwc_region_t &surface_damage);

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
    /* The mutex is used to protect changing the connection and power mode */
    std::mutex state_mutex;

    /* General callback functions for all displays */
    hwc2_callback callback_handler;

    /* The physical and virtual displays associated with this device */
    std::unordered_map<hwc2_display_t, hwc2_display> displays;

    /* String containing the dump output between calls */
    std::string dump_str;

    /* The associated adf hardware composer helper */
    struct adf_hwc_helper *adf_helper;

    int open_adf_display(adf_id_t adf_id);
};

struct hwc2_context {
    hwc2_device_t hwc2_device; /* must be first member in struct */

    hwc2_dev *hwc2_dev;
};

#endif /* ifndef _HWC2_H */
