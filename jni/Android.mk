LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libandroid \
	liblog

LOCAL_SRC_FILES := unionlib.c
LOCAL_MODULE := libunion

include $(BUILD_SHARED_LIBRARY)

