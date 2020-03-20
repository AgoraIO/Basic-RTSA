LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# Relative to the directory that contains Android.mk
AGORA_RTC_SDK_PATH := ../../../agora_rtsa_sdk

LOCAL_SRC_FILES := $(AGORA_RTC_SDK_PATH)/libs/$(TARGET_ARCH_ABI)/libagora-rtsa-sdk.so

# Relative to the current working directory when execute ndk-build
LOCAL_EXPORT_C_INCLUDES := ../../agora_rtsa_sdk/include

LOCAL_MODULE := libagora-rtsa-sdk

include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
## agora SDK wrapper
AGORA_SDK_WRAPPER_PATH := ../../AgoraSDKWrapper
AGORA_SDK_DEMOAPP_PATH := ../../AgoraSDKDemoApp

AGORA_SDK_WRAPPER_HEADER_PATH := ../AgoraSDKWrapper

LOCAL_CPPFLAGS := -std=c++11

LOCAL_C_INCLUDES += \
	$(AGORA_SDK_WRAPPER_HEADER_PATH)/.. \
	$(AGORA_SDK_WRAPPER_HEADER_PATH)/include \
	$(AGORA_SDK_WRAPPER_HEADER_PATH)/src/ \
	$(AGORA_SDK_WRAPPER_HEADER_PATH)/src/file_parser

LOCAL_SRC_FILES := \
	$(AGORA_SDK_WRAPPER_PATH)/src/file_parser/AACFileParser.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/file_parser/AudioFileParserFactory.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/file_parser/H264FileParser.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/file_parser/WavPcmFileParser.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/AgoraMediaTransporter.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/AudioFrameObserver.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/AudioPCMPuller.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/AutoResetEvent.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/bitbuffer.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/MediaCallbackImpl.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/MediaDataObserverReceiver.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/MediaDataReceiver.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/MediaDataSender.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/MediaSendTask.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/StatisticDump.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/Utils.cpp \
	$(AGORA_SDK_WRAPPER_PATH)/src/WAVHeader.cpp \
	$(AGORA_SDK_DEMOAPP_PATH)/src/main.cpp \

LOCAL_SHARED_LIBRARIES := libagora-rtsa-sdk

LOCAL_MODULE := agora_sdk_wrapper

include $(BUILD_EXECUTABLE)
