//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include "video_encoded_frame_receiver.h"

#include <errno.h>

VideoEncodedFrameReceiver::VideoEncodedFrameReceiver() : frameNum_(0), receivedBytes_(0) {}

VideoEncodedFrameReceiver::~VideoEncodedFrameReceiver() {
  if (file_) {
    fclose(file_);
  }
  file_ = nullptr;
}

bool VideoEncodedFrameReceiver::OnEncodedVideoImageReceived(
    const uint8_t* imageBuffer, size_t length,
    const agora::rtc::EncodedVideoFrameInfo& videoEncodedFrameInfo) {
  receivedBytes_ += length;
  //    printf("OnEncodedVideoImageReceived %d length %d, total length %d\n", frameNum_++, length,
  //           receivedBytes_);
  recvEncodedVideoImage_++;
  saveFile(imageBuffer, length);
  return true;
}

void VideoEncodedFrameReceiver::saveFile(const uint8_t* imageBuffer, size_t length) {
  if (!file_) {
    file_ = fopen("save_receiver.h264", "w");
  }

  if (!fwrite(imageBuffer, sizeof(uint8_t), length, file_)) {
    printf("Error writing save file: %s\n", std::strerror(errno));
  }
}
