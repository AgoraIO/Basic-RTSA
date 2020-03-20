//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once
#include <stdio.h>

#include "AgoraBase.h"

class VideoEncodedFrameReceiver : public agora::rtc::IVideoEncodedImageReceiver {
 public:
  VideoEncodedFrameReceiver();
  virtual ~VideoEncodedFrameReceiver();

 public:
  // inherit form agora::rtc::IVideoEncodedImageReceiver
  bool OnEncodedVideoImageReceived(
      const uint8_t* imageBuffer, size_t length,
      const agora::rtc::EncodedVideoFrameInfo& videoEncodedFrameInfo) override;

 private:
  void saveFile(const uint8_t* imageBuffer, size_t length);

 private:
  int frameNum_;
  int receivedBytes_;
  int recvEncodedVideoImage_{0};
  FILE* file_ = nullptr;
};
