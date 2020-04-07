//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include "video_encoded_frame_receiver.h"

#include <errno.h>
#include <stdint.h>
#include <string.h>

VideoEncodedFrameReceiver::VideoEncodedFrameReceiver() :
    file_(nullptr),
    verbose_(false),
    save_file_(false),
    received_encoded_video_frames_(0),
    received_total_bytes_(0) { }

VideoEncodedFrameReceiver::~VideoEncodedFrameReceiver() {
  if (file_) {
    fclose(file_);
  }
  file_ = nullptr;
}

bool VideoEncodedFrameReceiver::OnEncodedVideoImageReceived(
    const uint8_t* imageBuffer, size_t length,
    const agora::rtc::EncodedVideoFrameInfo& videoEncodedFrameInfo) {
  received_total_bytes_ += length;
  received_encoded_video_frames_++;

  if (verbose_) {
    printf("OnEncodedVideoImageReceived %d, frame length %lu, total length %d\n",
        received_encoded_video_frames_,
        length,
        received_total_bytes_);
  }

  if (save_file_) {
    writeEncodedVideoFrame(imageBuffer, length);
  }
  return true;
}

void VideoEncodedFrameReceiver::writeEncodedVideoFrame(const uint8_t* imageBuffer, size_t length) {
  if (!file_) {
    file_ = fopen("save_receiver.h264", "w");
  }

  if (!fwrite(imageBuffer, sizeof(uint8_t), length, file_)) {
    printf("Error writing save file: %s\n", strerror(errno));
  }
}
