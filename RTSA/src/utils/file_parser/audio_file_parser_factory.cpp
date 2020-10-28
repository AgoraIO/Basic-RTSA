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
#include "fixed_frame_length_audio_file_parser.h"

AudioFileParser::~AudioFileParser() {}

AudioFileParserFactory& AudioFileParserFactory::Instance() {
  static AudioFileParserFactory factory;
  return factory;
}

AudioFileParserFactory::AudioFileParserFactory() {}

AudioFileParserFactory::~AudioFileParserFactory() {}

std::unique_ptr<AudioFileParser> AudioFileParserFactory::createAudioFileParser(
    ParserConfig& config) {
  std::unique_ptr<AudioFileParser> parser;
  if (config.fileType == AUDIO_FILE_TYPE::AUDIO_FILE_OPUS) {
    parser = std::move(createOpusFileParser(config.filePath));
  } else if (config.fileType == AUDIO_FILE_TYPE::AUDIO_FILE_AACLC) {
    parser = std::move(createAACFileParser(config.filePath));
  } else if (config.fileType == AUDIO_FILE_TYPE::AUDIO_FILE_HEAAC) {
    parser = std::move(createHEAACFileParser(config.filePath));
  } else if (config.fileType == AUDIO_FILE_TYPE::AUDIO_FILE_PCM) {
    parser = std::move(createWavPcmFileParser(config.filePath));
  } else if (config.fileType == AUDIO_FILE_TYPE::AUDIO_FILE_FIX_LENGTH_FRAME) {
    parser = std::move(createFixFrameLengthFileParser(config.filePath, config.sampleRateHz,
                config.numberOfChannels, config.audioCodec, config.frameLength));
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

std::unique_ptr<AudioFileParser> AudioFileParserFactory::createFixFrameLengthFileParser(
    const char* filepath, int sampleRateHz, int numberOfChannels,
    agora::rtc::AUDIO_CODEC_TYPE codec, int frameLength) {
  std::unique_ptr<FixedFrameLengthAudioFileParser> parser(new FixedFrameLengthAudioFileParser(
      filepath, sampleRateHz, numberOfChannels, codec, frameLength));
  return std::move(parser);
}