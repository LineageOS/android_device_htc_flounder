# Copyright (C) 2016 The Android Open Source Project
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


# This hardware composer implements version 2.0. It is to be used as a reference
# guide for writing HWC 2.0 implementations. To enable, add
# "TARGET_USES_HWC2 := true" to flounder's BoardConfig.mk.
# It is a 2D HAL (no GL acceleration). Enabling it will degrade performance.
# To disable, remove "TARGET_USES_HWC2 := true" and run "make installclean"
ifeq ($(TARGET_USES_HWC2),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# HAL module implemenation, not prelinked and stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_MODULE := hwcomposer.$(TARGET_BOOTLOADER_BOARD_NAME)

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libutils

LOCAL_STATIC_LIBRARIES := \
	libadfhwc \
	libadf

LOCAL_SRC_FILES := \
	hwc2.cpp \
	hwc2_dev.cpp \
	hwc2_display.cpp \
	hwc2_config.cpp \
	hwc2_callback.cpp

LOCAL_MODLE_TAGS := optional

LOCAL_CFLAGS += -DLOG_TAG=\"hwcomposer\"

# The liboemcrypto.a prebuilt was built before we introduced the workaround for
# -Bsymbolic, so it was built without -fPIC. Work around this by using
# -Bsymbolic for projects that depend on it.
LOCAL_LDFLAGS := -Wl,-Bsymbolic

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))

endif # TARGET_USES_HWC2
