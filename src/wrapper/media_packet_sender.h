//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once

#include <string>

#include "api2/IAgoraService.h"
#include "api2/NGIAgoraMediaNodeFactory.h"
#include "utils/file_parser/audio_file_parser_factory.h"

class AudioFileParser;
class ConnectionWrapper;

struct SendConfig {
  uint32_t testDataLength{0};
  uint32_t lengthPerSend{0};
  uint32_t sendIntervalMs{0};
  bool audioTest{true};
};

class MediaPacketSender {
 public:
  MediaPacketSender(const SendConfig& config, int uid = 0);
  virtual ~MediaPacketSender();

  bool initialize(agora::base::IAgoraService* service,
                  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                  std::shared_ptr<ConnectionWrapper> connection);

  void sendPackets();

 private:
  SendConfig config_;
  agora::agora_refptr<agora::rtc::IMediaPacketSender> media_packet_sender_;
  agora::rtc::IMediaControlPacketSender* control_packet_sender_;

  int userId_{0};

  int sent_audio_packets_{0};
  int sent_audio_control_packets_{0};
  int sent_video_packets_{0};
  int sent_video_control_packets_{0};
};
