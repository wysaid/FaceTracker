#
# Created on: 2016-7-13
#     Author: Wang Yang
#       Mail: admin@wysaid.org
#

LOCAL_PATH := $(call my-dir)

###############################

#include $(CLEAR_VARS)

#OPENCV_INSTALL_MODULES:=on
#OPENCV_LIB_TYPE:=STATIC

#CV_ANDROID=/Users/wysaid/android_develop/opencv-android-sdk3.1

#include $(CV_ANDROID)/sdk/native/jni/OpenCV.mk

##############################

OPENCV_ROOT=$(LOCAL_PATH)/opencv

CVLIB_DIR=$(OPENCV_ROOT)/armeabi-v7a

include $(CLEAR_VARS)
LOCAL_MODULE := opencv

LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_java3.so
LOCAL_EXPORT_C_INCLUDES := $(OPENCV_ROOT) \
				   $(OPENCV_ROOT)/opencv2 \

include $(PREBUILT_SHARED_LIBRARY)

###############################


include $(CLEAR_VARS)

LOCAL_MODULE    := FaceTracker

#*********************** Tracker Library ****************************

TRACKER_ROOT=$(LOCAL_PATH)/../../../../..
TRACKER_SOURCE=$(TRACKER_ROOT)/src/lib
TRACKER_INCLUDE=$(TRACKER_ROOT)/include

LOCAL_C_INCLUDES += \
					$(TRACKER_ROOT) \
					$(TRACKER_INCLUDE) \
					$(LOCAL_PATH)/prebuilt_model \


LOCAL_SRC_FILES :=  \
			$(TRACKER_SOURCE)/CLM.cc \
			$(TRACKER_SOURCE)/FDet.cc \
			$(TRACKER_SOURCE)/PAW.cc \
			$(TRACKER_SOURCE)/Patch.cc \
			$(TRACKER_SOURCE)/FCheck.cc \
			$(TRACKER_SOURCE)/IO.cc \
			$(TRACKER_SOURCE)/PDM.cc \
			$(TRACKER_SOURCE)/Tracker.cc \
			\
			$(LOCAL_PATH)/FaceTrackerWrapper.cpp \



LOCAL_CPPFLAGS := -frtti -std=gnu++11
LOCAL_LDLIBS :=  -llog -ljnigraphics

LOCAL_CFLAGS    := -DANDROID_NDK -D__STDC_CONSTANT_MACROS -O3 -ffast-math -funroll-loops -march=armv7-a -mfloat-abi=softfp -mfpu=neon

LOCAL_SHARED_LIBRARIES := opencv

LOCAL_ARM_NEON := true
LOCAL_ARM_MODE := arm

include $(BUILD_SHARED_LIBRARY)



