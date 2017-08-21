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

#include <iomanip>
#include <sstream>

#include "hwc2.h"

hwc2_config::hwc2_config()
    : width(-1),
      height(-1),
      vsync_period(-1),
      dpi_x(-1),
      dpi_y(-1) { }

std::string hwc2_config::dump() const
{
    std::stringstream dmp;

    dmp << std::fixed << std::setprecision(1);

    dmp << "    Resolution: " << width << " x " << height;

    if (vsync_period != 0)
        dmp << " @ " << 1e9 / vsync_period << " Hz";

    dmp << "\n    DPI: " << dpi_x / 1000.0f << " x " << dpi_y / 1000.0f << "\n";

    return dmp.str();
}

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

int32_t hwc2_config::get_attribute(hwc2_attribute_t attribute) const
{
    switch (attribute) {
    case HWC2_ATTRIBUTE_WIDTH:
        return width;
    case HWC2_ATTRIBUTE_HEIGHT:
        return height;
    case HWC2_ATTRIBUTE_VSYNC_PERIOD:
        return vsync_period;
    case HWC2_ATTRIBUTE_DPI_X:
        return dpi_x;
    case HWC2_ATTRIBUTE_DPI_Y:
        return dpi_y;
    default:
        ALOGW("unknown attribute %u", attribute);
        return -1;
    }
}
