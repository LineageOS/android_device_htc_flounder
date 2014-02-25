ifneq ($(filter tegra132,$(TARGET_BOARD_PLATFORM)),)
ifneq ($(filter flounder flounder64,$(TARGET_DEVICE)),)

MY_LOCAL_PATH := $(call my-dir)

include $(MY_LOCAL_PATH)/hal/Android.mk

endif
endif
