include device/htc/flounder/BoardConfig.mk

TARGET_ARCH := arm64
TARGET_ARCH_VARIANT := armv8-a
TARGET_CPU_ABI := arm64-v8a
TARGET_CPU_ABI2 :=
TARGET_CPU_VARIANT := generic

TARGET_2ND_ARCH := arm
TARGET_2ND_ARCH_VARIANT := armv7-a-neon
TARGET_2ND_CPU_ABI := armeabi-v7a
TARGET_2ND_CPU_ABI2 := armeabi
TARGET_2ND_CPU_VARIANT := cortex-a15

# Disable 64-bit surfaceflinger until we have 64-bit GL libraries
TARGET_32_BIT_SURFACEFLINGER := true

# Disable emulator for "make dist" until there is a 64-bit qemu kernel
BUILD_EMULATOR := false
