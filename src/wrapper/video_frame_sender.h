//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once

#include <string>

#include "api2/IAgoraService.h"
#include "api2/NGIAgoraMediaNodeFactory.h"

class ConnectionWrapper;
class H264FileParser;

class VideoFrameSender {
 public:
  VideoFrameSender();
  virtual ~VideoFrameSender();
};

class VideoVP8FrameSender {
 public:
  VideoVP8FrameSender(const char* filepath);
  virtual ~VideoVP8FrameSender();

  bool initialize(agora::base::IAgoraService* service,
                  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                  std::shared_ptr<ConnectionWrapper> connection);

  void sendVideoFrames();

 private:
  std::string file_path_;
  agora::agora_refptr<agora::rtc::IVideoEncodedImageSender> video_encoded_image_sender_;
};

class VideoH264FileSender {
 public:
  VideoH264FileSender(const char* filepath);
  virtual ~VideoH264FileSender();

  bool initialize(agora::base::IAgoraService* service,
                  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                  std::shared_ptr<ConnectionWrapper> connection);

  void sendVideoFrames();

 private:
  std::string file_path_;
  agora::agora_refptr<agora::rtc::IVideoEncodedImageSender> video_encoded_image_sender_;
  std::unique_ptr<H264FileParser> file_parser_;
};

struct VideoPacket;

class VideoH264FramesSender {
 public:
  VideoH264FramesSender();
  virtual ~VideoH264FramesSender();

  bool initialize(agora::base::IAgoraService* service,
                  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                  std::shared_ptr<ConnectionWrapper> connection);

  void sendVideoFrames();

  int getSentFrameNum() { return sendNumFrames_; }
  int getSentBytes() { return sendBytes_; }

 private:
  bool sendOneFrame(struct VideoPacket* videoPacket);

 private:
  int sendBytes_{0};
  int sendNumFrames_{0};
  agora::agora_refptr<agora::rtc::IVideoEncodedImageSender> video_encoded_image_sender_;
};
