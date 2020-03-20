//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once
#include "utils/file_parser/audio_file_parser_factory.h"

class FixedFrameLengthAudioFileParser : public AudioFileParser {
 public:
  FixedFrameLengthAudioFileParser(const char* filepath, int sampleRateHz, int numberOfChannels,
                                  agora::rtc::AUDIO_CODEC_TYPE audioCodec, int frameLength);
  virtual ~FixedFrameLengthAudioFileParser();

 public:
  // AudioFileParser
  bool open() override;
  bool hasNext() override;

  void getNext(char* buffer, int* length) override;

  agora::rtc::AUDIO_CODEC_TYPE getCodecType() override;
  int getSampleRateHz() override;
  int getNumberOfChannels() override;

 private:
  void readData();

 private:
  static constexpr int BufferSize = 40960;

  char* filePath_;
  int sampleRateHz_;
  int numberOfChannels_;
  agora::rtc::AUDIO_CODEC_TYPE audioCodec_;
  int frameLength_;

  FILE* fileHandle_;
  unsigned char dataBuffer_[BufferSize] = {0};
  int currentBufPos_;
  int dataLengthInBuffer_;
  bool isEof_;
};
