LOCAL_PATH := $(call my-dir)

###    1. libagora-rtsa-sdk.so Generation    ###
include $(CLEAR_VARS)

# Relative to the directory that contains Android.mk
AGORA_RTC_SDK_PATH := ../../../agora_rtsa_sdk
LOCAL_SRC_FILES := $(AGORA_RTC_SDK_PATH)/libs/$(TARGET_ARCH_ABI)/libagora-rtsa-sdk.so

# Relative to the current working directory when execute ndk-build
LOCAL_EXPORT_C_INCLUDES_DIR := $(LOCAL_PATH)/../../../agora_rtsa_sdk/include

LOCAL_MODULE := libagora-rtsa-sdk
include $(PREBUILT_SHARED_LIBRARY)


###    2. libagora-sdk-demo.a Generation    ###
include $(CLEAR_VARS)

## agora SDK wrapper
AGORA_FILE_LIST := $(wildcard $(LOCAL_PATH)/../../src/wrapper/*.cpp)
AGORA_FILE_LIST += $(wildcard $(LOCAL_PATH)/../../src/wrapper_wrapper/*.cpp)
AGORA_FILE_LIST += $(wildcard $(LOCAL_PATH)/../../src/utils/*.cpp)
AGORA_FILE_LIST += $(wildcard $(LOCAL_PATH)/../../src/utils/file_parser/*.cpp)
AGORA_FILE_LIST += $(wildcard $(LOCAL_PATH)/../../src/rtc/*.cpp)

## please add ignored files to the following.
AGORA_IGNORE_FILE_LIST := %ogg_opus_file_parser.cpp

LOCAL_SRC_FILES := $(filter-out $(AGORA_IGNORE_FILE_LIST),$(AGORA_FILE_LIST))
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_EXPORT_C_INCLUDES_DIR)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../.. \
	$(LOCAL_PATH)/../../src \
	$(LOCAL_PATH)/../../src/utils \
	$(LOCAL_PATH)/../../src/utils/file_parser \
	$(LOCAL_PATH)/../../src/wrapper \
	$(LOCAL_PATH)/../../src/wrapper/../include \
	$(LOCAL_PATH)/../../src/wrapper_wrapper

LOCAL_EXPORT_C_INCLUDES_DIR += $(LOCAL_C_INCLUDES)

LOCAL_SHARED_LIBRARIES := libagora-rtsa-sdk
LOCAL_MODULE := agora-sdk-demo

include $(BUILD_STATIC_LIBRARY)


###    3. AgoraSDKDemoApp Generation    ###
include $(CLEAR_VARS)

## agora SDK Demo entry
AGORA_SDK_MAIN_PATH := ../../src/demo_main

LOCAL_C_INCLUDES += $(LOCAL_EXPORT_C_INCLUDES_DIR)

LOCAL_SRC_FILES := $(AGORA_SDK_MAIN_PATH)/main.cpp

LOCAL_SHARED_LIBRARIES := libagora-rtsa-sdk
LOCAL_STATIC_LIBRARIES := libagora-sdk-demo

LOCAL_MODULE := AgoraSDKDemoApp
include $(BUILD_EXECUTABLE)


###    4. AgoraSDKDemoApp Generation    ###
include $(CLEAR_VARS)

## agora SDK Demo entry
AGORA_SDK_MAIN_PATH := ../../src/dynamic_load_demo_main

LOCAL_C_INCLUDES += $(LOCAL_EXPORT_C_INCLUDES_DIR)

LOCAL_SRC_FILES := $(AGORA_SDK_MAIN_PATH)/main.cpp

LOCAL_SHARED_LIBRARIES := libagora-rtsa-sdk
LOCAL_STATIC_LIBRARIES := libagora-sdk-demo

LOCAL_MODULE := AgoraSDKDemoDlApp
include $(BUILD_EXECUTABLE)