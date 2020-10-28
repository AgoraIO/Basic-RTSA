//  Agora RTC/MEDIA SDK
//
//  Created by HK in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include "utils.h"
#include <memory>
#include <sstream>
#include "audio_frame_handler_factory.h"

std::unique_ptr<AudioPCMFrameHandler> AudioFrameHandlerFactory::createDefaultAudioHandler(int32_t uid) {
  AudioDataFetchParams param;
  if (default_handler_type_ == CHECKER_HANDLER) {
    param.save_file = false;
  } else {
    param.save_file = true;
    std::ostringstream oss;
    oss << "uid" << uid << "_" << defaultOutputFilePath_;
    param.file_saved_path = oss.str();
  }
  param.numberOfChannels = defaultNumberOfChannels_;
  param.sampleRateHz = defaultSampleRateHz_;
  return std::move(createAudioFrameHandler(param));
}

std::unique_ptr<AudioPCMFrameHandler> AudioFrameHandlerFactory::createAudioPcmCheckerHandler() {
  AudioDataFetchParams param;
  return std::move(createAudioFrameHandler(param));
}

std::unique_ptr<AudioPCMFrameHandler> AudioFrameHandlerFactory::createAudioPcmFileHandler(
    std::string& outputFilePath, uint32_t numberOfChannels, uint32_t sampleRateHz) {
  AudioDataFetchParams param;
  param.sampleRateHz = sampleRateHz;
  param.numberOfChannels = numberOfChannels;
  param.save_file = true;
  param.file_saved_path = outputFilePath;
  return std::move(createAudioFrameHandler(param));
}
