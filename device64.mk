ifeq ($(TARGET_PREBUILT_KERNEL),)
LOCAL_KERNEL := device/htc/flounder-kernel/flounder64/Image
else
LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/touch/touch_fusion.cfg:system/vendor/firmware/touch_fusion.cfg \
    $(LOCAL_PATH)/touch/maxim_fp35_64.bin:system/vendor/firmware/maxim_fp35.bin \
    $(LOCAL_PATH)/touch/touch_fusion.idc:system/usr/idc/touch_fusion.idc \
    $(LOCAL_PATH)/touch/touch_fusion_64:system/vendor/bin/touch_fusion

$(call inherit-product-if-exists, device/htc/flounder/device.mk)

$(call inherit-product-if-exists, hardware/nvidia/tegra132/tegra132.mk)
$(call inherit-product-if-exists, vendor/nvidia/proprietary-tegra132/tegra132-vendor.mk)
