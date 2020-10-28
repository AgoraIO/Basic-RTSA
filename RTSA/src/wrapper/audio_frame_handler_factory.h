//  Agora RTC/MEDIA SDK
//
//  Created by HK in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once
#include <memory>
#include "audio_pcm_frame_handler.h"

class AudioFrameHandlerFactory {
 public:
  enum HANDLER_TYPE : uint8_t {
    CHECKER_HANDLER,
    FILE_HANDLER
  };

 public:
  explicit AudioFrameHandlerFactory(HANDLER_TYPE default_type,
                                    std::string outputFilePath = "user_pcm_audio_data.wav",
                                    uint32_t numberOfChannels = 2, uint32_t sampleRateHz = 48000)
   : default_handler_type_(default_type),
     defaultOutputFilePath_(outputFilePath),
     defaultNumberOfChannels_(numberOfChannels),
     defaultSampleRateHz_(sampleRateHz) {}

  AudioFrameHandlerFactory() {}

 public:
  std::unique_ptr<AudioPCMFrameHandler> createDefaultAudioHandler(int32_t uid = 0);

  std::unique_ptr<AudioPCMFrameHandler> createAudioPcmCheckerHandler();

  std::unique_ptr<AudioPCMFrameHandler> createAudioPcmFileHandler(
      std::string& outputFilePath, uint32_t numberOfChannels, uint32_t sampleRateHz);

  HANDLER_TYPE getDefaultAudioHandlerType() const { return default_handler_type_; }

 private:
  HANDLER_TYPE default_handler_type_{CHECKER_HANDLER};
  std::string defaultOutputFilePath_;
  uint32_t defaultNumberOfChannels_;
  uint32_t defaultSampleRateHz_;
};
