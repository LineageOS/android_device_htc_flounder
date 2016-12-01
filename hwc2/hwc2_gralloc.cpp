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

#include "hwc2.h"

hwc2_gralloc::hwc2_gralloc()
{
    nvgr = dlopen("gralloc.tegra132.so", RTLD_LOCAL | RTLD_LAZY);
    LOG_ALWAYS_FATAL_IF(!nvgr, "failed to find module");

    *(void **)(&nvgr_is_valid) = dlsym(nvgr, "nvgr_is_valid");
    LOG_ALWAYS_FATAL_IF(!nvgr_is_valid, "failed to find nvgr_is_valid symbol");

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
