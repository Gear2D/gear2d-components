LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

SDL_PATH := ../SDL
GEAR2D_PATH := ../gear2d
LOCAL_MODULE := $(notdir $(LOCAL_PATH))
LOCAL_CPPFLAGS := -std=c++11 -fexceptions -DLOGTRACE

LOCAL_SRC_FILES := \
  $(wildcard $(LOCAL_PATH)/*.cc)

LOCAL_STATIC_LIBRARIES := gear2d_static

include $(BUILD_STATIC_LIBRARY)
