LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := nv_offload_visualizer.c

LOCAL_CFLAGS += -O2 -fvisibility=hidden

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	liblog \
	libtinyalsa

LOCAL_MODULE_RELATIVE_PATH := soundfx
LOCAL_MODULE := libnvvisualizer

LOCAL_MODULE_OWNER := htc

LOCAL_PROPRIETARY_MODULE := true

LOCAL_C_INCLUDES := \
	external/tinyalsa/include \
	$(call include-path-for, audio-effects)

include $(BUILD_SHARED_LIBRARY)

