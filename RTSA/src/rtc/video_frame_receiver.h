//
//  Agora Media SDK
//
//  Copyright (c) 2018 Agora IO. All rights reserved.
//

#pragma once

#include "NGIAgoraMediaNodeFactory.h"

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