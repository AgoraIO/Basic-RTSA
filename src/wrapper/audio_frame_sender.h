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

class AudioFrameSender {
 public:
  AudioFrameSender();
  virtual ~AudioFrameSender();

  virtual bool initialize(agora::base::IAgoraService* service,
                          agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                          std::shared_ptr<ConnectionWrapper> connection) = 0;

  virtual void sendAudioFrames() = 0;

  void setVerbose(bool verbose);

 protected:
  bool verbose_{false};
};

class EncodedAudioFrameSender : public AudioFrameSender {
 public:
  EncodedAudioFrameSender(const char* filepath, AUDIO_FILE_TYPE filetype);

  ~EncodedAudioFrameSender();

  bool initialize(agora::base::IAgoraService* service,
                  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                  std::shared_ptr<ConnectionWrapper> connection) override;

  void sendAudioFrames() override;

 private:
  std::string file_path;
  AUDIO_FILE_TYPE file_type;
  agora::agora_refptr<agora::rtc::IAudioEncodedFrameSender> audio_encoded_frame_sender_;
  std::unique_ptr<AudioFileParser> file_parser_;
  int64_t sent_audio_frames_{0};
};

class AudioPcmFrameSender : public AudioFrameSender {
 public:
  AudioPcmFrameSender(const char* filepath);

  ~AudioPcmFrameSender();

  bool initialize(agora::base::IAgoraService* service,
                  agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                  std::shared_ptr<ConnectionWrapper> connection) override;

  void sendAudioFrames() override;

 private:
  std::string file_path;
  std::unique_ptr<AudioFileParser> file_parser_;
  agora::agora_refptr<agora::rtc::IAudioPcmDataSender> audio_pcm_frame_ender_;
  int64_t sent_audio_frames_{0};
  bool verbose_{false};
};
