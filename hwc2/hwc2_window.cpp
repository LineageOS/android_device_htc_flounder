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

#include <cassert>

#include "hwc2.h"

hwc2_window::hwc2_window()
    : content(HWC2_WINDOW_CONTENT_EMPTY),
      z_order(0),
      lyr_id(0) { }

void hwc2_window::clear()
{
    content = HWC2_WINDOW_CONTENT_EMPTY;
}

hwc2_error_t hwc2_window::assign_client_target(uint32_t z_order)
{
    /* The client target will never rotate, scale or flip. Its buffer layout
     * could be changed before present display, so the window must support all
     * layouts. */
    if (!is_empty() || !has_requirements(HWC2_WINDOW_CAP_LAYOUTS))
        return HWC2_ERROR_UNSUPPORTED;

    content = HWC2_WINDOW_CONTENT_CLIENT_TARGET;
    this->z_order = z_order;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_window::assign_layer(uint32_t z_order, const hwc2_layer &lyr)
{
    if (!is_empty() || !has_layer_requirements(lyr))
        return HWC2_ERROR_UNSUPPORTED;

    content = HWC2_WINDOW_CONTENT_LAYER;
    this->z_order = z_order;
    this->lyr_id = lyr.get_id();

    return HWC2_ERROR_NONE;
}

bool hwc2_window::is_empty() const
{
    return content == HWC2_WINDOW_CONTENT_EMPTY;
}

bool hwc2_window::contains_client_target() const
{
    return content == HWC2_WINDOW_CONTENT_CLIENT_TARGET;
}

bool hwc2_window::contains_layer() const
{
    return content == HWC2_WINDOW_CONTENT_LAYER;
}

bool hwc2_window::has_layer_requirements(const hwc2_layer &lyr) const
{
    hwc_transform_t transform = lyr.get_transform();
    uint32_t reqs = 0;
    int32_t layout;

    layout = lyr.get_layout();
    if (layout < 0)
        return false;

    reqs |= layout;

    if (transform & HWC_TRANSFORM_ROT_90) {
        const void *surf;
        size_t surf_cnt;
        lyr.get_surfaces(&surf, &surf_cnt);

        if (surf_cnt > 1)
           reqs |= HWC2_WINDOW_CAP_ROTATE_PLANAR;
        else
           reqs |= HWC2_WINDOW_CAP_ROTATE_PACKED;
    }

    if (transform & (HWC_TRANSFORM_FLIP_H | HWC_TRANSFORM_FLIP_V))
        reqs |= HWC2_WINDOW_CAP_FLIP;

    if (lyr.get_scale_width() != 1 || lyr.get_scale_height() != 1)
        reqs |= HWC2_WINDOW_CAP_SCALE;

    if (lyr.is_yuv())
        reqs |= HWC2_WINDOW_CAP_YUV;

    return has_requirements(reqs);
}

bool hwc2_window::has_requirements(uint32_t requirements) const
{
    return (requirements & capabilities) == requirements;
}

void hwc2_window::set_capabilities(uint32_t capabilities)
{
    this->capabilities = capabilities;
}

bool hwc2_window::is_supported(const hwc2_layer &lyr)
{
    float source_crop_height = lyr.get_source_crop_height();
    float scale_width = lyr.get_scale_width();
    float scale_height = lyr.get_scale_height();

    if (!lyr.get_buffer_handle())
        return false;

    if (!lyr.get_adf_buffer_format())
        return false;

    if (lyr.is_stereo())
        return false;

    if (lyr.is_overlapped())
        return false;

    if (lyr.get_display_frame_width() < HWC2_WINDOW_MIN_DISPLAY_FRAME_WIDTH
            || lyr.get_display_frame_height() < HWC2_WINDOW_MIN_DISPLAY_FRAME_HEIGHT)
        return false;

    if (lyr.get_source_crop_width() < HWC2_WINDOW_MIN_SOURCE_CROP_WIDTH
            || source_crop_height < HWC2_WINDOW_MIN_SOURCE_CROP_HEIGHT)
        return false;

    if (scale_width < HWC2_WINDOW_MIN_DISPLAY_FRAME_SCALE
            || scale_height < HWC2_WINDOW_MIN_DISPLAY_FRAME_SCALE)
        return false;

    const void *surf;
    size_t surf_cnt;
    lyr.get_surfaces(&surf, &surf_cnt);

    if (lyr.get_transform() & HWC_TRANSFORM_ROT_90) {
        if (surf_cnt == 1 && scale_width == 1 && scale_height == 1
                && source_crop_height > HWC2_WINDOW_MAX_ROT_SRC_HEIGHT_NO_SCALE)
            return false;
        else if (source_crop_height > HWC2_WINDOW_MAX_ROT_SRC_HEIGHT)
            return false;
    }

    if (!lyr.is_source_crop_int_aligned())
        return false;

    return true;
}
