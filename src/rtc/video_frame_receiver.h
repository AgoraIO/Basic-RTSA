//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once
#include <stdio.h>

#include "AgoraBase.h"
#include "api2/NGIAgoraMediaNodeFactory.h"

class VideoFrameReceiver : public agora::rtc::IVideoSinkBase {
 public:
  VideoFrameReceiver() = default;
  virtual ~VideoFrameReceiver() = default;

 public:
  // agora::rtc::IVideoSinkBase
  int onFrame(const agora::media::VideoFrame& videoFrame) override;

 private:
  int64_t recvNumVideoFrames_{0};
  bool frameReceived_{false};
};
