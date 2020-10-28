//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once
#include <stdint.h>
#include <memory>

#include "AgoraBase.h"
#include "media_file_parser.h"

enum class AUDIO_FILE_TYPE : uint8_t {
  AUDIO_FILE_AACLC = 1,
  AUDIO_FILE_HEAAC,
  AUDIO_FILE_OPUS,
  AUDIO_FILE_PCM,
  AUDIO_FILE_FIX_LENGTH_FRAME
};

class AudioFileParserFactory {
 public:
  struct ParserConfig {
    AUDIO_FILE_TYPE fileType{AUDIO_FILE_TYPE::AUDIO_FILE_PCM};
    const char* filePath{nullptr};
    int sampleRateHz{48000};
    int numberOfChannels{2};
    agora::rtc::AUDIO_CODEC_TYPE audioCodec{agora::rtc::AUDIO_CODEC_PCMA};
    int frameLength{0};
  };

 public:
  static AudioFileParserFactory& Instance();
  ~AudioFileParserFactory();

  std::unique_ptr<AudioFileParser> createAudioFileParser(ParserConfig& config);

 private:
  std::unique_ptr<AudioFileParser> createAACFileParser(const char* filepath);
  std::unique_ptr<AudioFileParser> createHEAACFileParser(const char* filepath);
  std::unique_ptr<AudioFileParser> createOpusFileParser(const char* filepath);
  std::unique_ptr<AudioFileParser> createWavPcmFileParser(const char* filepath);
  std::unique_ptr<AudioFileParser> createFixFrameLengthFileParser(
      const char* filepath, int sampleRateHz, int numberOfChannels,
      agora::rtc::AUDIO_CODEC_TYPE codec, int frameLength);

 private:
  AudioFileParserFactory();
};
