//
//  Agora Media SDK
//
//  Copyright (c) 2018 Agora IO. All rights reserved.
//

#pragma once

#include "IAgoraMediaEngine.h"
#include "utils/common_utils.h"

struct CaptureVideoFrameResult {
  agora::media::IVideoFrameObserver::VideoFrame videoFrame;
};

struct RenderVideoFrameResult {
  agora::rtc::uid_t userId = {0};
  agora::rtc::conn_id_t connectionId;
  agora::media::IVideoFrameObserver::VideoFrame videoFrame;
};

class AsyncVideoFrameObserver : public agora::media::IVideoFrameObserver {
 public:
  AsyncVideoFrameObserver() = default;
  virtual ~AsyncVideoFrameObserver() = default;

  AsyncResult<CaptureVideoFrameResult>& AsyncCaptureVideoFrame() {
    return capture_video_frame_result_;
  }
  AsyncResult<RenderVideoFrameResult>& AsyncRenderVideoFrameResult() {
    return render_video_frame_result_;
  }

  int getRenderCount() { return render_count_; }
  agora::media::VIDEO_PIXEL_FORMAT setVideoPixelFormatPreference(
      agora::media::VIDEO_PIXEL_FORMAT new_type) {
    agora::media::VIDEO_PIXEL_FORMAT old_type = pixel_format_;
    pixel_format_ = new_type;
    return old_type;
  }

 private:
  bool onCaptureVideoFrame(agora::media::IVideoFrameObserver::VideoFrame& videoFrame) override;

  bool onRenderVideoFrame(agora::rtc::uid_t uid, agora::rtc::conn_id_t connectionId,
                          agora::media::IVideoFrameObserver::VideoFrame& videoFrame) override;

  agora::media::VIDEO_PIXEL_FORMAT getVideoPixelFormatPreference() override {
    return pixel_format_;
  }

 private:
  AsyncResult<CaptureVideoFrameResult> capture_video_frame_result_;
  AsyncResult<RenderVideoFrameResult> render_video_frame_result_;
  agora::media::VIDEO_PIXEL_FORMAT pixel_format_ = agora::media::VIDEO_PIXEL_I420;
  int capture_count_ = 0;
  int render_count_ = 0;
};
