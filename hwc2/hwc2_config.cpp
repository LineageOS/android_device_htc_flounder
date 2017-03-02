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

hwc2_config::hwc2_config()
    : width(-1),
      height(-1),
      vsync_period(-1),
      dpi_x(-1),
      dpi_y(-1) { }

int hwc2_config::set_attribute(hwc2_attribute_t attribute, int32_t value)
{
    switch (attribute) {
    case HWC2_ATTRIBUTE_WIDTH:
        width = value;
        break;
    case HWC2_ATTRIBUTE_HEIGHT:
        height = value;
        break;
    case HWC2_ATTRIBUTE_VSYNC_PERIOD:
        vsync_period = value;
        break;
    case HWC2_ATTRIBUTE_DPI_X:
        dpi_x = value;
        break;
    case HWC2_ATTRIBUTE_DPI_Y:
        dpi_y = value;
        break;
    default:
        ALOGW("unknown attribute %u", attribute);
        return -EINVAL;
    }

    return 0;
}
