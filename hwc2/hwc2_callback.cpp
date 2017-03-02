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

hwc2_callback::hwc2_callback()
   : hotplug_data(nullptr),
     hotplug(nullptr),
     refresh_data(nullptr),
     refresh(nullptr),
     vsync_data(nullptr),
     vsync(nullptr) { }

hwc2_error_t hwc2_callback::register_callback(
        hwc2_callback_descriptor_t descriptor,
        hwc2_callback_data_t callback_data, hwc2_function_pointer_t pointer)
{
    switch (descriptor) {
    case HWC2_CALLBACK_HOTPLUG:
        hotplug = (HWC2_PFN_HOTPLUG) pointer;
        hotplug_data = callback_data;
        break;
    case HWC2_CALLBACK_REFRESH:
        refresh = (HWC2_PFN_REFRESH) pointer;
        refresh_data = callback_data;
        break;
    case HWC2_CALLBACK_VSYNC:
        vsync = (HWC2_PFN_VSYNC) pointer;
        vsync_data = callback_data;
        break;
    default:
        ALOGE("unknown callback descriptor %u", descriptor);
        return HWC2_ERROR_BAD_PARAMETER;
    }

    return HWC2_ERROR_NONE;
}
