//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once
#include "AgoraBase.h"

class WavPcmFileWriter;

class AudioPCMFrameHandler {
 public:
  virtual ~AudioPCMFrameHandler() = default;

  virtual void preHandleAudio() = 0;
  virtual bool handlePcmData(void* payload_data,
                             const agora::rtc::AudioPcmDataInfo& audioFrameInfo) = 0;
  virtual void postHandleAudio() = 0;
};

class AudioPCMFrameFileHandler : public AudioPCMFrameHandler {
 public:
  AudioPCMFrameFileHandler(const std::string& outputFilePath, size_t numberOfChannels,
                           uint32_t sampleRateHz);
  virtual ~AudioPCMFrameFileHandler();

  void preHandleAudio() override;
  bool handlePcmData(void* payload_data,
                     const agora::rtc::AudioPcmDataInfo& audioFrameInfo) override;
  void postHandleAudio() override;

 private:
  std::unique_ptr<WavPcmFileWriter> file_writer_;
};

class AudioPCMFrameCheckerHandler : public AudioPCMFrameHandler {
 public:
  AudioPCMFrameCheckerHandler() : check_result_(false), frame_num_(0) {}
  virtual ~AudioPCMFrameCheckerHandler() = default;

  void preHandleAudio() override {}
  bool handlePcmData(void* payload_data,
                     const agora::rtc::AudioPcmDataInfo& audioFrameInfo) override;
  void postHandleAudio() override {}

  bool checkResult() { return check_result_; }

  int getReceivedFrameNumber() { return frame_num_; }

 private:
  bool check_result_;
  int frame_num_;
};
