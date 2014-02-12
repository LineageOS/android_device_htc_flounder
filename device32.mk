ifeq ($(TARGET_PREBUILT_KERNEL),)
LOCAL_KERNEL := device/htc/flounder-kernel/zImage-dtb
else
LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/touch/touch_fusion:system/vendor/bin/touch_fusion \
    $(LOCAL_PATH)/touch/touch_fusion.cfg:system/vendor/firmware/touch_fusion.cfg \
    $(LOCAL_PATH)/touch/maxim_fp35.bin:system/vendor/firmware/maxim_fp35.bin \
    $(LOCAL_PATH)/touch/touch_fusion.idc:system/usr/idc/touch_fusion.idc

$(call inherit-product-if-exists, device/htc/flounder/device.mk)
$(call inherit-product-if-exists, vendor/htc/flounder/device32-vendor.mk)

$(call inherit-product-if-exists, hardware/nvidia/tegra124/tegra124.mk)
$(call inherit-product-if-exists, vendor/nvidia/proprietary-tegra124/tegra124-vendor.mk)
