ifeq ($(TARGET_PREBUILT_KERNEL),)
LOCAL_KERNEL := device/htc/flounder-kernel/zImage-dtb
else
LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

$(call inherit-product-if-exists, device/htc/flounder/device.mk)

$(call inherit-product-if-exists, hardware/nvidia/tegra124/tegra124.mk)
$(call inherit-product-if-exists, vendor/nvidia/proprietary-tegra124/tegra124-vendor.mk)
