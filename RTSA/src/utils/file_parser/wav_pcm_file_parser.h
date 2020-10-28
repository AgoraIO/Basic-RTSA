//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-08.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//
#pragma once

#include <stdint.h>
#include <stdio.h>

#include "audio_file_parser_factory.h"
#include "utils/wav_header.h"

void makeWAVHeader(unsigned char _dst[44], const WavHeader& header);
void parseWAVHeader(const unsigned char _dst[44], WavHeader& header);

class WavPcmFileParser : public AudioFileParser {
 public:
  explicit WavPcmFileParser(const char* filepath);
  ~WavPcmFileParser();
  bool open() override;
  bool hasNext() override;
  int reset() override;

  int getNumberOfChannels() override;
  int getSampleRate();
  int getBitsPerSample() override;

  void getNext(char* buffer, int* length) override;
  agora::rtc::AUDIO_CODEC_TYPE getCodecType() override;
  int getSampleRateHz() override;

 private:
  void readData();

 private:
  static constexpr int BufferSize = 4096;

  char* wavFilePath_;
  FILE* wavFile_;
  unsigned char dataBuffer_[BufferSize] = {0};
  WavHeader wavHeader_;
  int32_t readedLength_;
  int32_t bufDataLength_;
  bool isEof_;
};
