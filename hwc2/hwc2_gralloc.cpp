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
#include <dlfcn.h>
#include <tegra_adf.h>

#include "hwc2.h"

#define NVGR_PIXEL_FORMAT_YUV420  0x100
#define NVGR_PIXEL_FORMAT_YUV422  0x101
#define NVGR_PIXEL_FORMAT_YUV422R 0x102
#define NVGR_PIXEL_FORMAT_UYVY    0x104
#define NVGR_PIXEL_FORMAT_NV12    0x106

#define NVGR_SURFACE_LAYOUT_PITCH        1
#define NVGR_SURFACE_LAYOUT_TILED        2
#define NVGR_SURFACE_LAYOUT_BLOCK_LINEAR 3

#define NVGR_SURFACE_SIZE 56

#define NVGR_GET_SURFACE(surfaces, idx) \
        ((void*)((char *) surfaces + (idx * NVGR_SURFACE_SIZE)))

/* The surface struct is defined in an NVIDIA binary. Accesses to the struct
 * will be done using member offsets */
#define NVGR_GET_STRUCT_MEMBER(ptr, member_type, member_offset) \
        (*((member_type *)((char *) ptr + member_offset)))

#define NVGR_SURFACE_OFFSET_LAYOUT 12

hwc2_gralloc::hwc2_gralloc()
{
    nvgr = dlopen("gralloc.tegra132.so", RTLD_LOCAL | RTLD_LAZY);
    LOG_ALWAYS_FATAL_IF(!nvgr, "failed to find module");

    *(void **)(&nvgr_is_valid) = dlsym(nvgr, "nvgr_is_valid");
    LOG_ALWAYS_FATAL_IF(!nvgr_is_valid, "failed to find nvgr_is_valid symbol");

    *(void **)(&nvgr_is_stereo) = dlsym(nvgr, "nvgr_is_stereo");
    LOG_ALWAYS_FATAL_IF(!nvgr_is_stereo, "failed to find nvgr_is_stereo"
            " symbol");

    *(void **)(&nvgr_is_yuv) = dlsym(nvgr, "nvgr_is_yuv");
    LOG_ALWAYS_FATAL_IF(!nvgr_is_stereo, "failed to find nvgr_is_yuv symbol");

    *(void **)(&nvgr_get_format) = dlsym(nvgr, "nvgr_get_format");
    LOG_ALWAYS_FATAL_IF(!nvgr_get_format, "failed to find nvgr_get_format"
            " symbol");

    *(void **)(&nvgr_get_surfaces) = dlsym(nvgr, "nvgr_get_surfaces");
    LOG_ALWAYS_FATAL_IF(!nvgr_get_surfaces, "failed to find nvgr_get_surfaces"
            " symbol");
}

hwc2_gralloc::~hwc2_gralloc()
{
    dlclose(nvgr);
}

const hwc2_gralloc &hwc2_gralloc::get_instance()
{
    static hwc2_gralloc instance;
    return instance;
}

bool hwc2_gralloc::is_valid(buffer_handle_t handle) const
{
    return nvgr_is_valid(handle);
}

bool hwc2_gralloc::is_stereo(buffer_handle_t handle) const
{
    return nvgr_is_stereo(handle);
}

bool hwc2_gralloc::is_yuv(buffer_handle_t handle) const
{
    return nvgr_is_yuv(handle);
}

int hwc2_gralloc::get_format(buffer_handle_t handle) const
{
    int format = nvgr_get_format(handle);

    switch (format) {
    case HAL_PIXEL_FORMAT_RGB_565:
        return DRM_FORMAT_BGR565;
    case NVGR_PIXEL_FORMAT_YUV420:
        return DRM_FORMAT_YUV420;
    case NVGR_PIXEL_FORMAT_YUV422:
        return DRM_FORMAT_YUV422;
    case NVGR_PIXEL_FORMAT_YUV422R:
        return TEGRA_ADF_FORMAT_YCbCr422R;
    case NVGR_PIXEL_FORMAT_UYVY:
        return DRM_FORMAT_UYVY;
    case NVGR_PIXEL_FORMAT_NV12:
        return DRM_FORMAT_NV12;
    default:
        return adf_fourcc_for_hal_pixel_format(format);
    }
}

void hwc2_gralloc::get_surfaces(buffer_handle_t handle,
        const void **surf, size_t *surf_cnt) const
{
    nvgr_get_surfaces(handle, surf, surf_cnt);
}

int32_t hwc2_gralloc::get_layout(const void *surf, uint32_t surf_idx) const
{
    uint32_t layout = NVGR_GET_STRUCT_MEMBER(NVGR_GET_SURFACE(surf, surf_idx),
            uint32_t, NVGR_SURFACE_OFFSET_LAYOUT);

    switch (layout) {
    case NVGR_SURFACE_LAYOUT_PITCH:
        return HWC2_WINDOW_CAP_PITCH;
    case NVGR_SURFACE_LAYOUT_TILED:
        return HWC2_WINDOW_CAP_TILED;
    case NVGR_SURFACE_LAYOUT_BLOCK_LINEAR:
        return HWC2_WINDOW_CAP_BLOCK_LINEAR;
    default:
        ALOGE("Unrecognized layout");
        return -1;
    }
}
