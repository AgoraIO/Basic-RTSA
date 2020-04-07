//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//
#pragma once
#include <sys/syscall.h>
#include <unistd.h>

#include "AgoraBase.h"
#include "api2/IAgoraService.h"
#include "api2/NGIAgoraRtcConnection.h"

#include "utils/file_parser/audio_file_parser_factory.h"

class AudioFileParser;
class ConnectionWrapper;

class MediaDataSender {
 public:
 public:
  explicit MediaDataSender(agora::base::IAgoraService* service, int uid = 0);
  virtual ~MediaDataSender();

  void setVerbose(bool verbose);
  bool connect(const char* channelId, agora::user_id_t userId);

  void sendAudioAACFile(const char* filepath, bool heaac);
  void sendAudioOpusFile(const char* filepath);

  void sendAudioPcmFile(const char* filepath);
  void sendAudioMediaPacket();

  void sendVideo();
  void sendVideoVp8File(const char* filepath);
  void sendVideoH264File(const char* filepath);
  void sendVideoMediaPacket();

 private:
  agora::agora_refptr<agora::rtc::IAudioEncodedFrameSender> createAudioEncodedFrameSender();
  void sendEncodedAudioFile(const char* filepath, AUDIO_FILE_TYPE filetype);

 protected:
  int uid_;
  pid_t pid_;
  int64_t tid_;
  int statisticPacketNum_;

  agora::base::IAgoraService* service_;
  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory_;
  std::shared_ptr<ConnectionWrapper> connection_;

  int sentNumVideoFrames_{0};

  bool verbose_{false};
};
