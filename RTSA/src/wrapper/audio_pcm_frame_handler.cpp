//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include "audio_pcm_frame_handler.h"

#include "utils/wav_pcm_file_writer.h"

AudioPCMFrameFileHandler::AudioPCMFrameFileHandler(const std::string& outputFilePath,
                                                   size_t numberOfChannels, uint32_t sampleRateHz) {
  file_writer_.reset(new WavPcmFileWriter(outputFilePath, numberOfChannels, sampleRateHz));
}

AudioPCMFrameFileHandler::~AudioPCMFrameFileHandler() = default;

void AudioPCMFrameFileHandler::preHandleAudio() { file_writer_->openWriter(); }

bool AudioPCMFrameFileHandler::handlePcmData(void* payload_data,
                                             const agora::rtc::AudioPcmDataInfo& audioFrameInfo) {
  return file_writer_->writeAudioPcmFrame(payload_data, audioFrameInfo.samplesOut, sizeof(int16_t));
}

void AudioPCMFrameFileHandler::postHandleAudio() { file_writer_->closeWriter(); }

bool AudioPCMFrameCheckerHandler::handlePcmData(
    void* payload_data, const agora::rtc::AudioPcmDataInfo& audioFrameInfo) {
  if (frame_num_ > 25) {
    int16_t* audiobuffer = reinterpret_cast<int16_t*>(payload_data);
    bool check = audiobuffer[1] > 0 || audiobuffer[12] > 0 || audiobuffer[23] > 0 ||
                 audiobuffer[34] > 0 || audiobuffer[45] > 0;
    check_result_ |= check;
  }
  ++frame_num_;
  return true;
}
