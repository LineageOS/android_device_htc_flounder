#
# Copyright (C) 2013 The Android Open-Source Project
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

# Wifi
PRODUCT_PACKAGES := \
    android.hardware.wifi@1.0-service \
    libwpa_client \
    hostapd \
    wpa_supplicant \
    wpa_supplicant.conf

PRODUCT_COPY_FILES := \
    $(LOCAL_PATH)/init.flounder.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.rc \
    $(LOCAL_PATH)/init.flounder.usb.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.usb.rc \
    $(LOCAL_PATH)/init.recovery.flounder.rc:root/init.recovery.flounder.rc \
    $(LOCAL_PATH)/fstab.flounder:$(TARGET_COPY_OUT_VENDOR)/etc/fstab.flounder \
    $(LOCAL_PATH)/ueventd.flounder.rc:$(TARGET_COPY_OUT_VENDOR)/ueventd.rc

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/touch/touch_fusion.cfg:$(TARGET_COPY_OUT_VENDOR)/firmware/touch_fusion.cfg \
    $(LOCAL_PATH)/touch/maxim_fp35.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/maxim_fp35.bin \
    $(LOCAL_PATH)/touch/touch_fusion.idc:$(TARGET_COPY_OUT_VENDOR)/usr/idc/touch_fusion.idc \
    $(LOCAL_PATH)/touch/touch_fusion:$(TARGET_COPY_OUT_VENDOR)/bin/touch_fusion \
    $(LOCAL_PATH)/touch/synaptics_dsx.idc:$(TARGET_COPY_OUT_VENDOR)/usr/idc/synaptics_dsx.idc

# headset keylayout
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/h2w_headset.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/h2w_headset.kl

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/tablet_core_hardware.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/tablet_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.wifi.passpoint.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.passpoint.xml \
    frameworks/native/data/etc/android.hardware.camera.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.xml \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.camera.full.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.full.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.audio.low_latency.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.audio.low_latency.xml \
    frameworks/native/data/etc/android.hardware.audio.pro.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.audio.pro.xml \
    frameworks/native/data/etc/android.hardware.nfc.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.nfc.xml \
    frameworks/native/data/etc/android.hardware.nfc.hce.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.nfc.hce.xml \
    frameworks/native/data/etc/android.hardware.nfc.hcef.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.nfc.hcef.xml \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth_le.xml \
    frameworks/native/data/etc/android.hardware.opengles.aep.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.opengles.aep.xml \
    frameworks/native/data/etc/android.software.midi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.midi.xml \
    $(LOCAL_PATH)/com.nvidia.nvsi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/com.nvidia.nvsi.xml

PRODUCT_COPY_FILES += \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_video.xml \
    $(LOCAL_PATH)/media_codecs.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs.xml \
    $(LOCAL_PATH)/media_codecs_performance.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_performance.xml \
    $(LOCAL_PATH)/media_profiles_V1_0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles_V1_0.xml

# Audio configuration
USE_XML_AUDIO_POLICY_CONF := 1

PRODUCT_COPY_FILES += \
    frameworks/av/services/audiopolicy/config/a2dp_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/a2dp_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/audio_policy_volumes.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_volumes.xml \
    frameworks/av/services/audiopolicy/config/default_volume_tables.xml:$(TARGET_COPY_OUT_VENDOR)/etc/default_volume_tables.xml \
    frameworks/av/services/audiopolicy/config/r_submix_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/r_submix_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/usb_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/usb_audio_policy_configuration.xml \
    $(LOCAL_PATH)/audio_effects.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_effects.xml \
    $(LOCAL_PATH)/audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_configuration.xml \
    $(LOCAL_PATH)/mixer_paths_0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/mixer_paths_0.xml

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/enctune.conf:system/etc/enctune.conf

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/nvaudio_conf.xml:system/etc/nvaudio_conf.xml \
    $(LOCAL_PATH)/nvcamera.conf:system/etc/nvcamera.conf \
    $(LOCAL_PATH)/bcmdhd.cal:system/etc/wifi/bcmdhd.cal \
    $(LOCAL_PATH)/bcmdhd_lte.cal:system/etc/wifi/bcmdhd_lte.cal

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/bluetooth/bcm4350b1.hcd:$(TARGET_COPY_OUT_VENDOR)/firmware/bcm4350b1.hcd \
    $(LOCAL_PATH)/bluetooth/BCM4354_003.001.012.0319.0690_ORC.hcd:$(TARGET_COPY_OUT_VENDOR)/firmware/bcm4350c0.hcd \
    $(LOCAL_PATH)/bluetooth/bcm4354.hcd:$(TARGET_COPY_OUT_VENDOR)/firmware/bcm4354.hcd \
    $(LOCAL_PATH)/bluetooth/bt_vendor.conf:$(TARGET_COPY_OUT_SYSTEM)/etc/bluetooth/bt_vendor.conf

# GPS configs
ifneq ($(filter lineage_flounder volantis volantisf, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/gps/bcm/gps.bcm47521.conf:system/etc/gps.bcm47521.conf \
    $(LOCAL_PATH)/gps/bcm/gpsconfig.xml:$(TARGET_COPY_OUT_VENDOR)/etc/gpsconfig.xml
else
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/gps/qct/gps.conf:system/etc/gps.conf
endif

# NFC feature + config files
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.nfc.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.nfc.xml \
    frameworks/native/data/etc/android.hardware.nfc.hce.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.nfc.hce.xml \
    device/htc/flounder/nfc/libnfc-brcm.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-brcm.conf \
    device/htc/flounder/nfc/libnfc-brcm-20795a10.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-brcm-20795a10.conf

# Bluetooth config files
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/bluetooth/bt_vendor.conf:system/etc/bluetooth/bt_vendor.conf

PRODUCT_AAPT_CONFIG := normal large xlarge
PRODUCT_AAPT_PREF_CONFIG := xhdpi

PRODUCT_CHARACTERISTICS := tablet,nosdcard

ifneq ($(filter lineage_flounder volantis volantisf, $(TARGET_PRODUCT)),)
# Wifi-Only overlays.
DEVICE_PACKAGE_OVERLAYS := \
    $(LOCAL_PATH)/wifi_only_overlay \
    $(LOCAL_PATH)/overlay
else
DEVICE_PACKAGE_OVERLAYS := \
    $(LOCAL_PATH)/lte_only_overlay \
    $(LOCAL_PATH)/overlay
endif

# NFC packages
PRODUCT_PACKAGES += \
    android.hardware.nfc@1.0-impl-bcm \
    android.hardware.nfc@1.0-service \
    nfc_nci.bcm2079x.default \
    NfcNci \
    Tag

# Audio HAL
PRODUCT_PACKAGES += \
    android.hardware.audio@2.0-impl \
    android.hardware.audio.effect@2.0-impl \
    android.hardware.soundtrigger@2.0-impl

# Bluetooth HAL
PRODUCT_PACKAGES += \
    libbt-vendor \
    android.hardware.bluetooth@1.0-impl

PRODUCT_PACKAGES += \
    librs_jni \
    com.android.future.usb.accessory

# Camera HAL
PRODUCT_PACKAGES += \
    camera.device@3.2-impl \
    android.hardware.camera.provider@2.4-impl

# Camera shim
PRODUCT_PACKAGES += \
    libcamera_shim \
    libEGL_vndk

# DRM HAL
PRODUCT_PACKAGES += \
    android.hardware.drm@1.0-impl \
    android.hardware.drm@1.0-service

# Dumpstate HAL
PRODUCT_PACKAGES += \
    android.hardware.dumpstate@1.0-service.flounder

# Gatekeeper HAL
PRODUCT_PACKAGES += \
    android.hardware.gatekeeper@1.0-impl \
    android.hardware.gatekeeper@1.0-service

# GNSS HAL
PRODUCT_PACKAGES += \
    android.hardware.gnss@1.0-impl

# Graphics HAL
PRODUCT_PACKAGES += \
    android.hardware.graphics.allocator@2.0-impl \
    android.hardware.graphics.allocator@2.0-service \
    android.hardware.graphics.composer@2.1-impl \
    android.hardware.graphics.mapper@2.0-impl \
    hwcomposer.flounder

# Health HAL
PRODUCT_PACKAGES += \
    android.hardware.health@1.0-impl \
    android.hardware.health@1.0-service

# Keymaster HAL
PRODUCT_PACKAGES += \
    android.hardware.keymaster@3.0-impl \
    android.hardware.keymaster@3.0-service

# Light HAL
PRODUCT_PACKAGES += \
    android.hardware.light@2.0-service.flounder

# Memtrack HAL
PRODUCT_PACKAGES += \
    android.hardware.memtrack@1.0-impl

# Power HAL
PRODUCT_PACKAGES += \
    android.hardware.power@1.0-impl \
    power.flounder

# RenderScript HAL
PRODUCT_PACKAGES += \
    android.hardware.renderscript@1.0-impl

# Sensors HAL
PRODUCT_PACKAGES += \
    android.hardware.sensors@1.0-impl \
    sensors.flounder

# Thermal HAL
PRODUCT_PACKAGES += \
    android.hardware.thermal@1.0-impl \
    thermal.flounder

# USB HAL
PRODUCT_PACKAGES += \
    android.hardware.usb@1.0-service.flounder

# Vibrator
PRODUCT_PACKAGES += \
    android.hardware.vibrator@1.0-impl

# Filesystem management tools
PRODUCT_PACKAGES += \
    fsck.f2fs mkfs.f2fs

PRODUCT_PROPERTY_OVERRIDES := \
    wifi.interface=wlan0 \
    ro.opengles.version=196609 \
    ro.sf.lcd_density=320 \
    ro.bt.bdaddr_path=/sys/module/flounder_bdaddress/parameters/bdaddress \
    ro.frp.pst=/dev/block/platform/sdhci-tegra.3/by-name/PST \
    ro.product.first_api_level=23 \
    ro.ril.def.agps.mode=1 \
    persist.tegra.compositor=glcomposer

# setup dalvik vm configs.
$(call inherit-product, frameworks/native/build/tablet-10in-xhdpi-2048-dalvik-heap.mk)
# N9 has only two CPUs, so drop dex2oat pressure after boot at runtime
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.dex2oat-threads=1

# for off charging mode
PRODUCT_PACKAGES += \
    charger \
    charger_res_images

# for audio
PRODUCT_PACKAGES += \
    audio.primary.flounder \
    audio.a2dp.default \
    audio.usb.default \
    audio.r_submix.default \
    libhtcacoustic
PRODUCT_PROPERTY_OVERRIDES += \
    ro.audio.monitorRotation=true

# for sound trigger on DSP
PRODUCT_PACKAGES += \
    sound_trigger.primary.flounder

# for keyboard key mappings
PRODUCT_PACKAGES += \
	VolantisKeyboard

# for launcher layout
#PRODUCT_PACKAGES += \
#    VolantisLayout

# drmservice prop
PRODUCT_PROPERTY_OVERRIDES += \
    drm.service.enabled=true

# facelock properties
PRODUCT_PROPERTY_OVERRIDES += \
    ro.facelock.black_timeout=700 \
    ro.facelock.det_timeout=2500 \
    ro.facelock.rec_timeout=3500 \
    ro.facelock.est_max_time=500

# Allows healthd to boot directly from charger mode rather than initiating a reboot.
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    ro.enable_boot_charger_mode=1

# Reduce client buffer size for fast audio output tracks
# and configure audio low latency for 128 frames per buffer
PRODUCT_PROPERTY_OVERRIDES += \
    af.fast_track_multiplier=1 \
    audio_hal.period_size=128

# Vendor seccomp policy files for media components:
PRODUCT_COPY_FILES += \
    device/htc/flounder/seccomp_policy/mediacodec.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediacodec.policy

# Verity dependencies (only on user builds for Lineage)
ifeq ($(TARGET_BUILD_VARIANT),user)
$(call inherit-product, build/target/product/verity.mk)
PRODUCT_SUPPORTS_BOOT_SIGNER := false
PRODUCT_SYSTEM_VERITY_PARTITION := /dev/block/platform/sdhci-tegra.3/by-name/APP
#PRODUCT_VENDOR_VERITY_PARTITION := /dev/block/platform/sdhci-tegra.3/by-name/VNR

# for warning
PRODUCT_PACKAGES += \
    slideshow \
    verity_warning_images
endif

# Add minidebug info the the boot image and the system server to support
# diagnosing native crashes, only on eng builds.
ifeq ($(TARGET_BUILD_VARIANT),eng)
    # Boot image.
    PRODUCT_DEX_PREOPT_BOOT_FLAGS += --generate-mini-debug-info
    # System server and some of its services.
    # Note: we cannot use PRODUCT_SYSTEM_SERVER_JARS, as it has not been expanded at this point.
    $(call add-product-dex-preopt-module-config,services,--generate-mini-debug-info)
    $(call add-product-dex-preopt-module-config,wifi-service,--generate-mini-debug-info)
endif

$(call inherit-product-if-exists, hardware/nvidia/tegra132/tegra132.mk)
$(call inherit-product-if-exists, vendor/nvidia/proprietary-tegra132/tegra132-vendor.mk)
$(call inherit-product, vendor/htc/flounder-common/flounder-common-vendor.mk)
ifeq ($(TARGET_PRODUCT),lineage_flounder_lte)
    $(call inherit-product, vendor/htc/flounder_lte/flounder_lte-vendor.mk)
else
    $(call inherit-product, vendor/htc/flounder/flounder-vendor.mk)
endif
$(call inherit-product-if-exists, hardware/broadcom/wlan/bcmdhd/firmware/bcm4354/device-bcm.mk)
$(call inherit-product-if-exists, vendor/htc/flounder/audio/lifevibes/lvve/device-vendor-lvve.mk)
$(call inherit-product-if-exists, vendor/htc/flounder/audio/tfa/device-vendor-tfa.mk)

# Add dependency of the proprietary keystore.flounder module.
PRODUCT_PACKAGES += \
    libkeymaster_messages
