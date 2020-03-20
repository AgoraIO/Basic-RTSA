//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include "video_frame_receiver.h"

int VideoFrameReceiver::onFrame(const agora::media::VideoFrame& videoFrame) {
#if 0
  AGO_LOG("Receive video frame(%d) %d x %d\n", connectionTest_->recvNumVideoFrames_,
      videoFrame.width(), videoFrame.height());
#endif
  ++recvNumVideoFrames_;
  frameReceived_ = true;

  return 0;
}
