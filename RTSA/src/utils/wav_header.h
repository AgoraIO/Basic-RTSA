//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-09.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once

#include <stdint.h>
#include <cstring>

struct WavHeader {
  char tag[5] = {'R', 'I', 'F', 'F', 0};
  int32_t streamLength;
  char typeTag[5] = {'W', 'A', 'V', 'E', 0};
  char fmtTag[5] = {'f', 'm', 't', ' ', 0};
  int32_t cksize = 0x10;
  int16_t codec = 1;  // 1 -> pcm
  int16_t numberOfChannels = 0;
  int32_t sampleRateHz = 0;
  int32_t bytesPerSecond = 0;
  int16_t bytesPerSample = 0;
  int16_t bitsPerSample = 16;
  char dataTag[5] = {'d', 'a', 't', 'a', 0};
  int32_t dataLength = 0;
};

void makeWAVHeader(unsigned char _dst[44], const WavHeader& header);
void parseWAVHeader(const unsigned char _dst[44], WavHeader& header);

static void put_le32(unsigned char* _dst, uint32_t _x) {
  _dst[0] = (unsigned char)(_x & 0xFF);
  _dst[1] = (unsigned char)(_x >> 8 & 0xFF);
  _dst[2] = (unsigned char)(_x >> 16 & 0xFF);
  _dst[3] = (unsigned char)(_x >> 24 & 0xFF);
}

static uint32_t get_le32(const unsigned char* _dst) {
  uint32_t value = _dst[0];
  value |= ((uint32_t)_dst[1]) << 8;
  value |= ((uint32_t)_dst[2]) << 16;
  value |= ((uint32_t)_dst[3]) << 24;
  return value;
}

static void put_le16(unsigned char* _dst, uint16_t _x) {
  _dst[0] = (unsigned char)(_x & 0xFF);
  _dst[1] = (unsigned char)(_x >> 8 & 0xFF);
}

static uint16_t get_le16(const unsigned char* _dst) {
  uint16_t value = _dst[0];
  value |= ((uint16_t)_dst[1]) << 8;
  return value;
}
