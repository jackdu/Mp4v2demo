LOCAL_PATH := $(call my-dir)
#==============================================================================
include $(CLEAR_VARS)
LOCAL_SRC_FILES := libmp4v2.so
LOCAL_MODULE := mp4v2
include $(PREBUILT_SHARED_LIBRARY)
#==============================================================================

include $(CLEAR_VARS)
LOCAL_MODULE    := Mp4v2demo
LOCAL_SRC_FILES := Mp4v2demo.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc $(LOCAL_PATH)/inc/mp4v2

LOCAL_LDLIBS := -llog

LOCAL_SHARED_LIBRARIES := mp4v2

include $(BUILD_SHARED_LIBRARY)
