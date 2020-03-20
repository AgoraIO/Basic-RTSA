//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once
#include <stdint.h>
#include <memory>

#include "AgoraBase.h"

class AudioFileParser {
 public:
  virtual ~AudioFileParser();
  virtual bool open() = 0;
  virtual bool hasNext() = 0;
  virtual void getNext(char* buffer, int* length) = 0;
  virtual agora::rtc::AUDIO_CODEC_TYPE getCodecType() = 0;
  virtual int getSampleRateHz() = 0;
  virtual int getNumberOfChannels() = 0;
  virtual int getBitsPerSample() { return 0; }
  virtual int reset() { return 0; }
};

enum class AUDIO_FILE_TYPE : uint8_t {
  AUDIO_FILE_AACLC = 1,
  AUDIO_FILE_HEAAC,
  AUDIO_FILE_OPUS,
  AUDIO_FILE_PCM,
  AUDIO_FILE_FIX_LENGTH_FRAME
};

class AudioFileParserFactory {
 public:
  static AudioFileParserFactory& Instance();
  ~AudioFileParserFactory();

  std::unique_ptr<AudioFileParser> createAudioFileParser(const char* filepath,
                                                         AUDIO_FILE_TYPE filetype);

 private:
  std::unique_ptr<AudioFileParser> createAACFileParser(const char* filepath);
  std::unique_ptr<AudioFileParser> createHEAACFileParser(const char* filepath);
  std::unique_ptr<AudioFileParser> createOpusFileParser(const char* filepath);
  std::unique_ptr<AudioFileParser> createWavPcmFileParser(const char* filepath);

 private:
  AudioFileParserFactory();
};
