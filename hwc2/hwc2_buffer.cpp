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

#include "hwc2.h"

hwc2_buffer::hwc2_buffer()
    : dataspace(),
      blend_mode(HWC2_BLEND_MODE_NONE) { }

hwc2_error_t hwc2_buffer::set_dataspace(android_dataspace_t dataspace)
{
    this->dataspace = dataspace;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_buffer::set_blend_mode(hwc2_blend_mode_t blend_mode)
{
    if (blend_mode == HWC2_BLEND_MODE_INVALID) {
        ALOGE("invalid blend mode");
        return HWC2_ERROR_BAD_PARAMETER;
    }

    this->blend_mode = blend_mode;

    return HWC2_ERROR_NONE;
}
