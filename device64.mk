ifeq ($(TARGET_PREBUILT_KERNEL),)
LOCAL_KERNEL := device/htc/flounder-kernel/flounder64/Image
else
LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

$(call inherit-product-if-exists, device/htc/flounder/device.mk)

$(call inherit-product-if-exists, hardware/nvidia/tegra132/tegra132.mk)
$(call inherit-product-if-exists, vendor/nvidia/proprietary-tegra132/tegra132-vendor.mk)
