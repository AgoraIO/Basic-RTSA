//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-08.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//
#pragma once
#include <stdio.h>
#include <string>

#include "api2/NGIAgoraLocalUser.h"

class AudioPCMFrameHandler;

class AudioPCMPuller {
 public:
  AudioPCMPuller(const agora::rtc::AudioPcmDataInfo& audioFrameInfo, size_t numberOfChannels,
                 size_t bytesPerSample, uint32_t sampleRateHz, uint64_t duration,
                 agora::rtc::ILocalUser* audioFramePuller);
  virtual ~AudioPCMPuller();

  void setAudioPcmFrameHandler(std::unique_ptr<AudioPCMFrameHandler> handler);
  void startPullAudioPCM();

  bool handlePcmData(void* payload_data, const agora::rtc::AudioPcmDataInfo& audioFrameInfo);

 private:
  agora::rtc::AudioPcmDataInfo audioFrameInfo_;
  size_t numberOfChannels_;
  size_t bytesPerSample_;
  uint32_t sampleRateHz_;
  uint64_t duration_;
  agora::rtc::ILocalUser* audioFramePuller_;
  uint64_t endTime_;

  std::unique_ptr<AudioPCMFrameHandler> auido_frame_handler_;
};
