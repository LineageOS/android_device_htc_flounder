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
#include <tegra_dc_ext.h>
#include <tegrafb.h>
#include <tegra_adf.h>
#include <android-base/macros.h>
#include <sstream>

#include "hwc2.h"

static bool operator==(const hwc_rect_t &r1, const hwc_rect_t &r2)
{
    return r1.left == r2.left && r1.top == r2.top && r1.right == r2.right
            && r1.bottom == r2.bottom;
}

static bool operator!=(const hwc_rect_t &r1, const hwc_rect_t &r2)
{
    return !(r1 == r2);
}

static bool operator==(const hwc_frect_t &r1, const hwc_frect_t &r2)
{
    return r1.left == r2.left && r1.top == r2.top && r1.right == r2.right
            && r1.bottom == r2.bottom;
}

static bool operator!=(const hwc_frect_t &r1, const hwc_frect_t &r2)
{
    return !(r1 == r2);
}

static bool cmp_region(std::vector<hwc_rect_t> &r1, const hwc_region_t &r2)
{
    if (r1.size() != r2.numRects)
        return false;

    return std::equal(r1.begin(), r1.end(), r2.rects);
}

hwc2_buffer::hwc2_buffer()
    : handle(),
      acquire_fence(-1),
      dataspace(),
      display_frame(),
      source_crop(),
      z_order(0),
      surface_damage(),
      blend_mode(HWC2_BLEND_MODE_NONE),
      plane_alpha(1.0),
      transform(),
      visible_region(),
      previous_format(0),
      modified(true) { }

hwc2_buffer::~hwc2_buffer()
{
    close_acquire_fence();
}

std::string hwc2_buffer::dump() const
{
    std::stringstream dmp;

    dmp << "    Buffer: " << std::hex << handle << "/" << std::dec
            << acquire_fence << "    Z: " << z_order << "\n";
    dmp << "    Display Frame: [" << display_frame.left << ", "
            << display_frame.top << ", " << display_frame.right << ", "
            << display_frame.bottom << "]\n";
    dmp << "    Source Crop: [" << source_crop.left << ", "
            << source_crop.top << ", " << source_crop.right << ", "
            << source_crop.bottom << "]\n";
    dmp << "    Transform: " << getTransformName(transform)
            << "    Blend Mode: " << getBlendModeName(blend_mode);

    if (plane_alpha != 1.0f)
        dmp << "    Alpha: " << plane_alpha;

    dmp << "\n";

    return dmp.str();
}

hwc2_error_t hwc2_buffer::decompress()
{
    int ret = hwc2_gralloc::get_instance().decompress(handle, acquire_fence,
            &acquire_fence);
    if (ret < 0) {
        ALOGE("failed to decompress buffer: %s", strerror(ret));
        return HWC2_ERROR_NO_RESOURCES;
    }
    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_buffer::get_adf_post_props(
        struct tegra_adf_flip_windowattr *win_attr,
        struct adf_buffer_config *adf_buf, size_t win_idx,
        size_t buf_idx, uint32_t z_order) const
{
    if (!hwc2_gralloc::get_instance().is_valid(handle)) {
        ALOGE("invalid buffer handle");
        return HWC2_ERROR_NO_RESOURCES;
    }

    hwc2_error_t ret = get_adf_buf_config(adf_buf);
    if (ret != HWC2_ERROR_NONE) {
        ALOGE("failed to set adf buf configs");
        return ret;
    }

    ret = get_adf_win_attr(win_attr, win_idx, buf_idx, z_order);
    if (ret != HWC2_ERROR_NONE) {
        ALOGE("failed to set win attr");
        return ret;
    }

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_buffer::get_adf_buf_config(
        struct adf_buffer_config *adf_buf) const
{
    const hwc2_gralloc& gralloc = hwc2_gralloc::get_instance();

    adf_buf->overlay_engine = 0;
    adf_buf->w = (uint32_t) (ceil(source_crop.right) - ceil(source_crop.left));
    adf_buf->h = (uint32_t) (ceil(source_crop.bottom) - ceil(source_crop.top));
    adf_buf->format = gralloc.get_format(handle);

    const void *surf;
    size_t surf_cnt;
    gralloc.get_surfaces(handle, &surf, &surf_cnt);
    if (!surf || surf_cnt == 0) {
        ALOGE("failed to get surfaces");
        return HWC2_ERROR_NO_RESOURCES;
    }

    adf_buf->n_planes = surf_cnt;

    int fd;
    gralloc.get_dma_buf(surf, 0, &fd);
    for (size_t idx = 0; idx < arraysize(adf_buf->fd); idx++) {
        if (idx < adf_buf->n_planes)
            adf_buf->fd[idx] = fd;
        else
            adf_buf->fd[idx] = -1;
    }

    adf_buf->pitch[0] = gralloc.get_pitch(surf, 0);
    adf_buf->offset[0] = gralloc.get_offset(surf, 0);

    if (surf_cnt == 1) {
        adf_buf->pitch[1] = 0;
        adf_buf->pitch[2] = 0;
        adf_buf->offset[1] = 0;
        adf_buf->offset[2] = 0;
    } else {
        switch (surf_cnt) {
        case 2:
            adf_buf->pitch[1] = gralloc.get_pitch(surf, 1);
            adf_buf->pitch[2] = gralloc.get_pitch(surf, 1);
            adf_buf->offset[1] = gralloc.get_offset(surf, 1);
            adf_buf->offset[2] = 0;
            break;
        case 3:
            adf_buf->pitch[1] = gralloc.get_pitch(surf, 1);
            adf_buf->pitch[2] = gralloc.get_pitch(surf, 1);
            adf_buf->offset[1] = gralloc.get_offset(surf, 1);
            adf_buf->offset[2] = gralloc.get_offset(surf, 2);
            break;
        default:
            LOG_ALWAYS_FATAL("Invalid surface count. There must be between 1 to"
                    " 3 surfaces per buffer.");
            break;
        }
    }

    adf_buf->acquire_fence = acquire_fence;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_buffer::get_adf_win_attr(
        struct tegra_adf_flip_windowattr *win_attr, size_t win_idx,
        size_t buf_idx, uint32_t z_order) const
{
    const hwc2_gralloc& gralloc = hwc2_gralloc::get_instance();
    const void *surf;
    size_t surf_cnt;
    uint32_t layout;

    win_attr->win_index = win_idx;
    win_attr->buf_index = buf_idx;

    switch (blend_mode) {
    case HWC2_BLEND_MODE_PREMULTIPLIED:
        win_attr->blend = TEGRA_ADF_BLEND_PREMULT;
        break;
    case HWC2_BLEND_MODE_COVERAGE:
        win_attr->blend = TEGRA_ADF_BLEND_COVERAGE;
        break;
    case HWC2_BLEND_MODE_NONE:
        win_attr->blend = TEGRA_ADF_BLEND_NONE;
        break;
    default:
        ALOGW("invalid blend mode %d", blend_mode);
        win_attr->blend = TEGRA_ADF_BLEND_NONE;
    }

    win_attr->x = ((uint32_t) ceil(source_crop.left)) << 12;
    win_attr->y = ((uint32_t) ceil(source_crop.top)) << 12;
    win_attr->out_x = display_frame.left;
    win_attr->out_y = display_frame.top;
    win_attr->out_w = display_frame.right - display_frame.left;
    win_attr->out_h = display_frame.bottom - display_frame.top;
    win_attr->z = z_order;
    win_attr->flags = 0;

    gralloc.get_surfaces(handle, &surf, &surf_cnt);
    if (!surf || surf_cnt == 0) {
        ALOGE("failed to get surfaces");
        return HWC2_ERROR_NO_RESOURCES;
    }

    layout = gralloc.get_layout(surf, 0);
    if (layout == HWC2_WINDOW_CAP_TILED)
        win_attr->flags |= TEGRA_FB_WIN_FLAG_TILED;

    if (layout == HWC2_WINDOW_CAP_BLOCK_LINEAR) {
        win_attr->flags |= TEGRA_DC_EXT_FLIP_FLAG_BLOCKLINEAR;
        win_attr->block_height_log2 = gralloc.get_block_height_log2(surf, 0);
    }

    win_attr->flags |= TEGRA_DC_EXT_FLIP_FLAG_GLOBAL_ALPHA;
    win_attr->global_alpha = (uint8_t) (plane_alpha * 0xFF);

    if (transform & HWC_TRANSFORM_FLIP_H)
        win_attr->flags |= TEGRA_DC_EXT_FLIP_FLAG_INVERT_H;

    if (transform & HWC_TRANSFORM_FLIP_V)
        win_attr->flags |= TEGRA_DC_EXT_FLIP_FLAG_INVERT_V;

    if (transform & HWC_TRANSFORM_ROT_90) {
        win_attr->flags |= TEGRA_DC_EXT_FLIP_FLAG_SCAN_COLUMN;
        win_attr->flags ^= TEGRA_DC_EXT_FLIP_FLAG_INVERT_V;
    }

    return HWC2_ERROR_NONE;
}

void hwc2_buffer::close_acquire_fence()
{
    if (acquire_fence >= 0) {
        close(acquire_fence);
        acquire_fence = -1;
    }
}

uint32_t hwc2_buffer::get_adf_buffer_format() const
{
    if (!handle)
        return 0;

    return hwc2_gralloc::get_instance().get_format(handle);
}

uint32_t hwc2_buffer::get_layout() const
{
    const hwc2_gralloc &gralloc = hwc2_gralloc::get_instance();
    const void *surf;
    size_t surf_cnt;

    if (!handle)
        return 0;

    gralloc.get_surfaces(handle, &surf, &surf_cnt);

    return gralloc.get_layout(surf, 0);
}

int hwc2_buffer::get_display_frame_width() const
{
    return display_frame.right - display_frame.left;
}

int hwc2_buffer::get_display_frame_height() const
{
    return display_frame.bottom - display_frame.top;
}

float hwc2_buffer::get_source_crop_width() const
{
    return source_crop.right - source_crop.left;
}

float hwc2_buffer::get_source_crop_height() const
{
    return source_crop.bottom - source_crop.top;
}

float hwc2_buffer::get_scale_width() const
{
    float source_crop_width = get_source_crop_width();
    if (source_crop_width == 0)
        return -1.0;

    return get_display_frame_width() / source_crop_width;
}

float hwc2_buffer::get_scale_height() const
{
    float source_crop_height = get_source_crop_height();
    if (source_crop_height == 0)
        return -1.0;

    return get_display_frame_height() / source_crop_height;
}

void hwc2_buffer::get_surfaces(const void **surf, size_t *surf_cnt) const
{
    if (!handle)
        *surf_cnt = 0;
    else
        hwc2_gralloc::get_instance().get_surfaces(handle, surf, surf_cnt);
}

bool hwc2_buffer::is_source_crop_int_aligned() const
{
    return (source_crop.left == ceil(source_crop.left)
            && source_crop.top == ceil(source_crop.top)
            && source_crop.right == ceil(source_crop.right)
            && source_crop.bottom == ceil(source_crop.bottom));
}

bool hwc2_buffer::is_stereo() const
{
    if (!handle)
        return false;

    return hwc2_gralloc::get_instance().is_stereo(handle);
}

bool hwc2_buffer::is_yuv() const
{
    if (!handle)
        return false;

    return hwc2_gralloc::get_instance().is_yuv(handle);
}

bool hwc2_buffer::is_overlapped() const
{
    if (visible_region.size() != 1)
        return true;

    if (visible_region.at(0).left != display_frame.left
            || visible_region.at(0).top != display_frame.top
            || visible_region.at(0).right != display_frame.right
            || visible_region.at(0).bottom != display_frame.bottom)
        return true;

    return false;
}

hwc2_error_t hwc2_buffer::set_buffer(buffer_handle_t handle,
        int32_t acquire_fence)
{
    /* Only check if non-null buffers are valid. Layer buffers are determined to
     * be non-null in hwc2_layer. Client target buffers can be null and should
     * not produce an error. */
    if (handle && !hwc2_gralloc::get_instance().is_valid(handle)) {
        ALOGE("invalid buffer handle");
        return HWC2_ERROR_BAD_PARAMETER;
    }

    close_acquire_fence();

    this->handle = handle;
    this->acquire_fence = acquire_fence;

    uint32_t format = get_adf_buffer_format();
    modified = modified || format != previous_format;
    previous_format = format;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_buffer::set_dataspace(android_dataspace_t dataspace)
{
    modified = modified || dataspace != this->dataspace;
    this->dataspace = dataspace;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_buffer::set_display_frame(const hwc_rect_t &display_frame)
{
    modified = modified || display_frame != this->display_frame;
    this->display_frame = display_frame;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_buffer::set_source_crop(const hwc_frect_t &source_crop)
{
    modified = modified || source_crop != this->source_crop;
    this->source_crop = source_crop;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_buffer::set_z_order(uint32_t z_order)
{
    modified = modified || z_order != this->z_order;
    this->z_order = z_order;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_buffer::set_surface_damage(const hwc_region_t &surface_damage)
{
    this->surface_damage.clear();
    for (size_t idx = 0; idx < surface_damage.numRects; idx++)
        this->surface_damage.push_back(surface_damage.rects[idx]);

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_buffer::set_blend_mode(hwc2_blend_mode_t blend_mode)
{
    if (blend_mode == HWC2_BLEND_MODE_INVALID) {
        ALOGE("invalid blend mode");
        return HWC2_ERROR_BAD_PARAMETER;
    }

    modified = modified || blend_mode != this->blend_mode;
    this->blend_mode = blend_mode;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_buffer::set_plane_alpha(float plane_alpha)
{
    modified = modified || plane_alpha != this->plane_alpha;
    this->plane_alpha = plane_alpha;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_buffer::set_transform(const hwc_transform_t transform)
{
    modified = modified || transform != this->transform;
    this->transform = transform;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_buffer::set_visible_region(const hwc_region_t &visible_region)
{
    modified = modified || !cmp_region(this->visible_region, visible_region);

    this->visible_region.clear();
    for (size_t idx = 0; idx < visible_region.numRects; idx++)
        this->visible_region.push_back(visible_region.rects[idx]);

    return HWC2_ERROR_NONE;
}
