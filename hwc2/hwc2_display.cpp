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

#include "hwc2.h"

uint64_t hwc2_display::display_cnt = 0;

hwc2_display::hwc2_display(hwc2_display_t id, int adf_intf_fd,
        const struct adf_device &adf_dev)
    : id(id),
      adf_intf_fd(adf_intf_fd),
      adf_dev(adf_dev) { }

hwc2_display::~hwc2_display()
{
    close(adf_intf_fd);
    adf_device_close(&adf_dev);
}

hwc2_display_t hwc2_display::get_next_id()
{
    return display_cnt++;
}
