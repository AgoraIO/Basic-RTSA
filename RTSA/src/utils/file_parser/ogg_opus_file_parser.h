//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once
#include <ogg/ogg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/file_parser/audio_file_parser_factory.h"

struct OggOpusFile;

struct decode_context {
  int current_packet{0};
  int total_length{0};

  int nsamples{0};
  int nchannels;
  int format;

  unsigned char packet[960 * 2 * 2]{0};
  long bytes{0};
  long b_o_s{0};
  long e_o_s{0};

  ogg_int64_t granulepos;
  ogg_int64_t packetno;
};

class OggOpusFileParser : public AudioFileParser {
 public:
  explicit OggOpusFileParser(const char* filepath);
  virtual ~OggOpusFileParser();

 public:
  // AudioFileParser
  bool open() override;
  bool hasNext() override;

  void getNext(char* buffer, int* length) override;

  agora::rtc::AUDIO_CODEC_TYPE getCodecType() override;
  int getSampleRateHz() override;
  int getNumberOfChannels() override;

 private:
  void loadMetaInfo(OggOpusFile* oggOpusFile);

 private:
  char* oggOpusFilePath_;
  OggOpusFile* oggOpusFile_;
  int sampleRateHz_;
  int numberOfChannels_;
  decode_context decode_context_;
  bool eof;
};
