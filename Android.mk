GEAR2D_COMPONENTS :=
  
include $(call all-subdir-makefiles)

include $(CLEAR_VARS)

LOCAL_MODULE    := gear2d_components
LOCAL_WHOLE_STATIC_LIBRARIES := $(GEAR2D_COMPONENTS)

include $(BUILD_STATIC_LIBRARY)
