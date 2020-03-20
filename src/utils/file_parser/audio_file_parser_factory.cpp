//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//
#include "audio_file_parser_factory.h"

#include <memory>

#if defined(__linux__) && !defined(__ANDROID__)
#include "ogg_opus_file_parser.h"
#endif
#include "aac_file_parser.h"
#include "wav_pcm_file_parser.h"

AudioFileParser::~AudioFileParser() {}

AudioFileParserFactory& AudioFileParserFactory::Instance() {
  static AudioFileParserFactory factory;
  return factory;
}

AudioFileParserFactory::AudioFileParserFactory() {}

AudioFileParserFactory::~AudioFileParserFactory() {}

std::unique_ptr<AudioFileParser> AudioFileParserFactory::createAudioFileParser(
    const char* filepath, AUDIO_FILE_TYPE filetype) {
  std::unique_ptr<AudioFileParser> parser;
  if (filetype == AUDIO_FILE_TYPE::AUDIO_FILE_OPUS) {
    parser = std::move(createOpusFileParser(filepath));
  } else if (filetype == AUDIO_FILE_TYPE::AUDIO_FILE_AACLC) {
    parser = std::move(createAACFileParser(filepath));
  } else if (filetype == AUDIO_FILE_TYPE::AUDIO_FILE_HEAAC) {
    parser = std::move(createHEAACFileParser(filepath));
  } else if (filetype == AUDIO_FILE_TYPE::AUDIO_FILE_PCM) {
    parser = std::move(createWavPcmFileParser(filepath));
  }
  return std::move(parser);
}

std::unique_ptr<AudioFileParser> AudioFileParserFactory::createAACFileParser(const char* filepath) {
  std::unique_ptr<AACFileParser> parser(new AACFileParser(filepath));
  return std::move(parser);
}

std::unique_ptr<AudioFileParser> AudioFileParserFactory::createHEAACFileParser(
    const char* filepath) {
  std::unique_ptr<HEAACFileParser> parser(new HEAACFileParser(filepath));
  return std::move(parser);
}

std::unique_ptr<AudioFileParser> AudioFileParserFactory::createOpusFileParser(
    const char* filepath) {
#if defined(__linux__) && !defined(__ANDROID__)
  std::unique_ptr<OggOpusFileParser> parser(new OggOpusFileParser(filepath));
  return std::move(parser);
#else
  return nullptr;
#endif
}

std::unique_ptr<AudioFileParser> AudioFileParserFactory::createWavPcmFileParser(
    const char* filepath) {
  std::unique_ptr<WavPcmFileParser> parser(new WavPcmFileParser(filepath));
  return std::move(parser);
}
