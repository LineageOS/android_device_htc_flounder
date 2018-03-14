# Lineage specific changes

# Inherit car device configuration
$(call inherit-product, device/generic/car/common/car.mk)

# Inherit device configuration
$(call inherit-product, device/htc/flounder/lineage_flounder.mk)

## Device identifier. This must come after all inclusions
PRODUCT_NAME := car_flounder
PRODUCT_DEVICE := flounder
PRODUCT_BRAND := google
PRODUCT_MODEL := Nexus 9
