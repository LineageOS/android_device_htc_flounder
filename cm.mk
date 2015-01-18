# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

# Enhanced NFC
$(call inherit-product, vendor/cm/config/nfc_enhanced.mk)

# Inherit device configuration
$(call inherit-product, device/htc/flounder/aosp_shamu.mk)

DEVICE_PACKAGE_OVERLAYS += device/htc/flounder/overlay-cm

## Device identifier. This must come after all inclusions
PRODUCT_NAME := cm_flounder
PRODUCT_BRAND := google
PRODUCT_MODEL := Nexus 9
