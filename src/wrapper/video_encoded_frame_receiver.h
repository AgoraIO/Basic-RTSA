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

 public:
  void SetVerbose(bool verbose);
  int GetReceivedVideoFrames() { return received_encoded_video_frames_; }

 private:
  void writeEncodedVideoFrame(const uint8_t* imageBuffer, size_t length);

 private:
  FILE* file_;
  bool verbose_;
  bool save_file_;
  int received_total_bytes_;
  int received_encoded_video_frames_{0};
};
