# Copyright (C) 2014 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# This file is the build configuration that is shared by all products
# based on the flounder device
#

# (TODO): Remove this hack once all devices in the source tree
# have switched to a separate vendor.img.
TARGET_COPY_OUT_VENDOR := vendor

PRODUCT_RUNTIMES := runtime_libart_default

# HACK: force volantis back to 32-bit system server for now
PRODUCT_COPY_FILES += system/core/rootdir/init.zygote32_64.rc:root/init.zygote32_64.rc
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.zygote=zygote32_64

$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, device/htc/flounder/device.mk)
