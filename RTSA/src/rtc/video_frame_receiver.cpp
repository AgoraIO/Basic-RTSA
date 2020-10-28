//
//  Agora Media SDK
//
//  Copyright (c) 2018 Agora IO. All rights reserved.
//

#include "video_frame_receiver.h"

int VideoFrameReceiver::onFrame(const agora::media::VideoFrame& videoFrame) {
  ++recvNumVideoFrames_;
  frameReceived_ = true;
  return 0;
}
