//
//  Agora Media SDK
//
//  Copyright (c) 2018 Agora IO. All rights reserved.
//

#include "video_frame_observer.h"

bool AsyncVideoFrameObserver::onCaptureVideoFrame(VideoFrame& videoFrame) {
  capture_video_frame_result_.AppendResult({videoFrame});
  capture_count_ ++;
  return true;
}

bool AsyncVideoFrameObserver::onRenderVideoFrame(agora::rtc::uid_t uid, agora::rtc::conn_id_t connectionId,
                        VideoFrame& videoFrame) {
  RenderVideoFrameResult result;
  result.userId = uid;
  result.connectionId = connectionId;
  result.videoFrame = videoFrame;
  render_video_frame_result_.AppendResult(result);
  render_count_++;
  return true;
}
