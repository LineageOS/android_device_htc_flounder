# CyanogenMod Specific Changes

# Boot animation
TARGET_SCREEN_HEIGHT := 2048
TARGET_SCREEN_WIDTH := 1536

# Camera
PRODUCT_PACKAGES += \
    Snap

PRODUCT_PROPERTY_OVERRIDES += \
    persist.camera.cpp.duplication=false

# Gello
PRODUCT_PACKAGES += \
    Gello

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_tablet_wifionly.mk)

# Inherit device configuration
$(call inherit-product, device/htc/flounder/aosp_flounder.mk)

$(call inherit-product-if-exists, vendor/htc/flounder/device-vendor.mk)

BOARD_NEEDS_VENDORIMAGE_SYMLINK := true

# Inline kernel building
TARGET_KERNEL_SOURCE := kernel/htc/flounder
TARGET_KERNEL_CONFIG := lineage_flounder_defconfig
BOARD_KERNEL_IMAGE_NAME := Image.gz-dtb

# Extra Packages
PRODUCT_PACKAGES += \
    com.android.nfc_extras

# CM Overlays
DEVICE_PACKAGE_OVERLAYS += device/htc/flounder/overlay-cm

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRODUCT_NAME=flounder \
    BUILD_FINGERPRINT=google/volantis/flounder:7.1.1/N4F26X/3821951:user/release-keys \
    PRIVATE_BUILD_DESC="volantis-user 7.1.1 N4F26X 3821951 release-keys"

## Device identifier. This must come after all inclusions
PRODUCT_NAME := lineage_flounder
PRODUCT_BRAND := google
PRODUCT_MODEL := Nexus 9
