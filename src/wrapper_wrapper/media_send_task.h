//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once
#include <string>

#include "api2/IAgoraService.h"

class MediaSendTask {
 public:
  MediaSendTask(agora::base::IAgoraService* service, std::string threadName, int cycles,
                bool sendAudio, bool sendVideo, bool sendMediaPacket, int uid);
  virtual ~MediaSendTask();
  virtual void Run();
  void setAudioCodecType(agora::rtc::AUDIO_CODEC_TYPE audioCodec);
  void setVideoCodecType(agora::rtc::VIDEO_CODEC_TYPE videoCodec, bool multiSlice);

 private:
  agora::base::IAgoraService* service_;
  std::string threadName_;
  int cycles_;
  bool sendAudio_;
  bool sendVideo_;
  bool mediaPacket_;
  agora::rtc::AUDIO_CODEC_TYPE audioCodec_;
  agora::rtc::VIDEO_CODEC_TYPE videoCodec_;
  bool multiSlice_;
  int uid_;
};
