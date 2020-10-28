//
//  Agora Media SDK
//
//  Copyright (c) 2018 Agora IO. All rights reserved.
//

#include "video_encoded_image_receiver.h"

bool AsyncVideoEncodedImageReceiver::OnEncodedVideoImageReceived(
    const uint8_t* imageBuffer, size_t length,
    const agora::rtc::EncodedVideoFrameInfo& encodedVideoFrameInfo) {
  encoded_video_frame_result_.AppendResult({encodedVideoFrameInfo});
  receive_count_++;
  return true;
}