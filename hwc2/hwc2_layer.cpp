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

#include <inttypes.h>
#include <cutils/log.h>

#include "hwc2.h"

uint64_t hwc2_layer::layer_cnt = 0;

hwc2_layer::hwc2_layer(hwc2_layer_t id)
    : id(id),
      buffer(),
      comp_type(HWC2_COMPOSITION_INVALID) { }

hwc2_error_t hwc2_layer::set_comp_type(hwc2_composition_t comp_type)
{
    hwc2_error_t ret = HWC2_ERROR_NONE;

    switch (comp_type) {
    case HWC2_COMPOSITION_CLIENT:
    case HWC2_COMPOSITION_DEVICE:
    case HWC2_COMPOSITION_CURSOR:
    case HWC2_COMPOSITION_SOLID_COLOR:
        break;

    case HWC2_COMPOSITION_SIDEBAND:
        ret = HWC2_ERROR_UNSUPPORTED;
        break;

    case HWC2_COMPOSITION_INVALID:
    default:
        ALOGE("lyr %" PRIu64 ": invalid composition type %u", id, comp_type);
        ret = HWC2_ERROR_BAD_PARAMETER;
    }

    this->comp_type = comp_type;
    return ret;
}

hwc2_error_t hwc2_layer::set_dataspace(android_dataspace_t dataspace)
{
    return buffer.set_dataspace(dataspace);
}

hwc2_error_t hwc2_layer::set_blend_mode(hwc2_blend_mode_t blend_mode)
{
    return buffer.set_blend_mode(blend_mode);
}

hwc2_layer_t hwc2_layer::get_next_id()
{
    return layer_cnt++;
}
