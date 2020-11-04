#!/bin/bash
#
# Copyright (C) 2016 The CyanogenMod Project
# Copyright (C) 2017-2020 The LineageOS Project
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

set -e

if [ -z "${DEVICE}" ]; then
    DEVICE=flounder
fi
DEVICE_COMMON=flounder-common
VENDOR=htc

# Load extract_utils and do some sanity checks
MY_DIR="${BASH_SOURCE%/*}"
if [[ ! -d "${MY_DIR}" ]]; then MY_DIR="${PWD}"; fi

LINEAGE_ROOT="${MY_DIR}/../../.."

HELPER="${LINEAGE_ROOT}/vendor/lineage/build/tools/extract_utils.sh"
if [ ! -f "${HELPER}" ]; then
    echo "Unable to find helper script at ${HELPER}"
    exit 1
fi
source "${HELPER}"

function blob_fixup() {
    case "${1}" in
        vendor/lib/libglcore.so)
            ;&
        vendor/lib64/libglcore.so)
            patchelf --add-needed "libutilscallstack.so" "${2}"
            ;;
        vendor/lib/hw/camera.tegra132.so)
            ;&
        vendor/lib/soundfx/libLifevibes_lvverx.so)
            ;&
        vendor/lib/soundfx/libLifevibes_lvvetx.so)
            ;&
        vendor/lib/libaudioavp.so)
            ;&
        vendor/lib/libfrsdk.so)
            ;&
        vendor/lib/libnvcam_imageencoder.so)
            ;&
        vendor/lib/libnvcamerahdr.so)
            ;&
        vendor/lib/libnvcapclk.so)
            ;&
        vendor/lib/libnvcompute.so)
            ;&
        vendor/lib/libnvisp_v3.so)
            ;&
        vendor/lib/libnvmm_camera_v3.so)
            ;&
        vendor/lib/libnvmmlite_video.so)
            ;&
        vendor/lib/libnvodm_imager.so)
            ;&
        vendor/lib/libnvvicsi_v3.so)
            ;&
        vendor/lib/libopencv24_tegra.so)
            ;&
        vendor/lib/libtsechdcp.so)
            sed -i "s/libm.so/libw.so/g" "${2}"
            ;;
    esac
}

# Default to sanitizing the vendor folder before extraction
CLEAN_VENDOR=true

SECTION=
KANG=

while [ "${#}" -gt 0 ]; do
    case "${1}" in
        -n | --no-cleanup )
                CLEAN_VENDOR=false
                ;;
        -k | --kang )
                KANG="--kang"
                ;;
        -s | --section )
                SECTION="${2}"; shift
                CLEAN_VENDOR=false
                ;;
        * )
                SRC="${1}"
                ;;
    esac
    shift
done

if [ -z "${SRC}" ]; then
    SRC=adb
fi

# Initialize the helper for common device
setup_vendor "${DEVICE_COMMON}" "${VENDOR}" "${LINEAGE_ROOT}" true "${CLEAN_VENDOR}"

extract "${MY_DIR}/common-proprietary-files.txt" "${SRC}" "${SECTION}"

if [ -s "${MY_DIR}/proprietary-files.txt" ] || [ -s "${MY_DIR}/../${DEVICE}/device-proprietary-files.txt" ]; then
    # Reinitialize the helper for device
    setup_vendor "${DEVICE}" "${VENDOR}" "${LINEAGE_ROOT}" false "${CLEAN_VENDOR}"

    extract "${MY_DIR}/proprietary-files.txt" "${SRC}" "${SECTION}"
    extract "${MY_DIR}/../${DEVICE}/device-proprietary-files.txt" "${SRC}" "${SECTION}"
fi

"${MY_DIR}/setup-makefiles.sh"
