//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include <string.h>

#include "fixed_frame_length_audio_file_parser.h"

FixedFrameLengthAudioFileParser::FixedFrameLengthAudioFileParser(
    const char* filepath, int sampleRateHz, int numberOfChannels,
    agora::rtc::AUDIO_CODEC_TYPE audioCodec, int frameLength)
    : filePath_(strdup(filepath)),
      sampleRateHz_(sampleRateHz),
      numberOfChannels_(numberOfChannels),
      audioCodec_(audioCodec),
      frameLength_(frameLength),
      fileHandle_(nullptr),
      currentBufPos_(0),
      dataLengthInBuffer_(0),
      isEof_(false) {}

FixedFrameLengthAudioFileParser::~FixedFrameLengthAudioFileParser() {
  if (fileHandle_) {
    fclose(fileHandle_);
  }
  free(static_cast<void*>(filePath_));
}

bool FixedFrameLengthAudioFileParser::open() {
  fileHandle_ = fopen(filePath_, "rb");
  return fileHandle_ != nullptr;
}

void FixedFrameLengthAudioFileParser::readData() {
  if (isEof_) {
    return;
  }
  if (dataLengthInBuffer_ - currentBufPos_ > 0) {
    memmove(dataBuffer_, dataBuffer_ + currentBufPos_, dataLengthInBuffer_ - currentBufPos_);
    dataLengthInBuffer_ = dataLengthInBuffer_ - currentBufPos_;
    currentBufPos_ = 0;
  }
  size_t readsize =
      fread(dataBuffer_ + dataLengthInBuffer_, 1, BufferSize - dataLengthInBuffer_, fileHandle_);
  if (readsize == 0) {
    isEof_ = true;
  }
  dataLengthInBuffer_ += readsize;
}

bool FixedFrameLengthAudioFileParser::hasNext() {
  if (dataLengthInBuffer_ - currentBufPos_ < frameLength_) {
    readData();
  }
  return dataLengthInBuffer_ - currentBufPos_ > frameLength_;
}

void FixedFrameLengthAudioFileParser::getNext(char* buffer, int* length) {
  memcpy(buffer, dataBuffer_ + currentBufPos_, frameLength_);
  *length = frameLength_;
  currentBufPos_ += frameLength_;
}

agora::rtc::AUDIO_CODEC_TYPE FixedFrameLengthAudioFileParser::getCodecType() { return audioCodec_; }

int FixedFrameLengthAudioFileParser::getSampleRateHz() { return sampleRateHz_; }

int FixedFrameLengthAudioFileParser::getNumberOfChannels() { return numberOfChannels_; }
