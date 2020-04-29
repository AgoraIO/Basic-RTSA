//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once

#include <string>

#include "IAgoraService.h"
#include "NGIAgoraMediaNodeFactory.h"
#include "utils/file_parser/h264_file_parser.h"
#include "utils/file_parser/video_file_parser_factory.h"
#include "video_frame_sender_internal.h"
#include "wrapper/connection_wrapper.h"
 
class VideoFrameSender {
 public:
  VideoFrameSender();
  virtual ~VideoFrameSender();

  virtual bool initialize(agora::base::IAgoraService* service,
                          agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                          std::shared_ptr<ConnectionWrapper> connection) = 0;

  virtual void sendVideoFrames() = 0;

  void setVerbose(bool verbose);

 protected:
  bool verbose_{false};
};

class ExternalVideoFrameSender : public VideoFrameSender {
 public:
  ExternalVideoFrameSender(const char* filepath);

  ~ExternalVideoFrameSender();

  bool initialize(agora::base::IAgoraService* service,
                  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                  std::shared_ptr<ConnectionWrapper> connection) override;

  void sendVideoFrames() override;

 public:
  void setVideoFrameInfo(int width, int height, int framerate, agora::media::VIDEO_PIXEL_FORMAT format);

 private:
  std::string file_path;
  int width_{0};
  int height_{0};
  int framerate_{0};
  agora::media::VIDEO_PIXEL_FORMAT format_{agora::media::VIDEO_PIXEL_I420};
  agora::agora_refptr<agora::rtc::IVideoFrameSender> video_external_frame_sender_;
  std::unique_ptr<VideoFileParser> file_parser_;
  int64_t sent_video_frames_{0};
};

class VideoVP8FrameSender : public VideoFrameSender {
 public:
  VideoVP8FrameSender(const char* filepath);
  virtual ~VideoVP8FrameSender();

  bool initialize(agora::base::IAgoraService* service,
                  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                  std::shared_ptr<ConnectionWrapper> connection) override;

  void sendVideoFrames() override;

 private:
  std::string file_path_;
  agora::agora_refptr<agora::rtc::IVideoEncodedImageSender> video_encoded_image_sender_;
};

class VideoH264FileSender : public VideoFrameSender {
 public:
  VideoH264FileSender(const char* filepath);
  virtual ~VideoH264FileSender();

  bool initialize(agora::base::IAgoraService* service,
                  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                  std::shared_ptr<ConnectionWrapper> connection) override;

  void sendVideoFrames() override;

 private:
  std::string file_path_;
  agora::agora_refptr<agora::rtc::IVideoEncodedImageSender> video_encoded_image_sender_;
  std::unique_ptr<H264FileParser> file_parser_;
};

class VideoH264FramesSender : public VideoFrameSender {
 public:
  VideoH264FramesSender();
  virtual ~VideoH264FramesSender();

  bool initialize(agora::base::IAgoraService* service,
                  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                  std::shared_ptr<ConnectionWrapper> connection) override;

  void sendVideoFrames() override;

  int getSentFrameNum() { return sendNumFrames_; }
  int getSentBytes() { return sendBytes_; }

 private:
  bool sendOneFrame(struct VideoPacket* videoPacket);

 private:
  int sendBytes_{0};
  int sendNumFrames_{0};
  agora::agora_refptr<agora::rtc::IVideoEncodedImageSender> video_encoded_image_sender_;
};