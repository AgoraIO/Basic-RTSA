//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-09.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#include "wav_header.h"

/*Make a header for a 48 kHz, stereo, signed, 16-bit little-endian PCM WAV.*/
void makeWAVHeader(unsigned char _dst[44], const WavHeader& header) {
  /*The chunk sizes are set to 0x7FFFFFFF by default.
   Many, though not all, programs will interpret this to mean the duration is
   "undefined", and continue to read from the file so long as there is actual
   data.*/
  static const unsigned char WAV_HEADER_TEMPLATE[44] = {
      'R',  'I',  'F',  'F',  0xFF, 0xFF, 0xFF, 0x7F, 'W',  'A',  'V',  'E',  'f',  'm',  't',
      ' ',  0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x44, 0xAC, 0x00, 0x00, 0x10, 0xB1,
      0x02, 0x00, 0x04, 0x00, 0x10, 0x00, 'd',  'a',  't',  'a',  0xFF, 0xFF, 0xFF, 0x7F};
  memcpy(_dst, WAV_HEADER_TEMPLATE, sizeof(WAV_HEADER_TEMPLATE));
  put_le32(_dst + 4, header.dataLength + 36);
  put_le16(_dst + 20, header.codec);
  put_le16(_dst + 22, header.numberOfChannels);
  put_le32(_dst + 24, header.sampleRateHz);
  put_le32(_dst + 28, header.bytesPerSecond);
  put_le16(_dst + 32, header.bytesPerSample);
  put_le16(_dst + 34, header.bitsPerSample);
  put_le32(_dst + 40, header.dataLength);
}

void parseWAVHeader(const unsigned char _dst[44], WavHeader& header) {
  strncpy(reinterpret_cast<char*>(header.tag), reinterpret_cast<const char*>(_dst), 4);
  strncpy(reinterpret_cast<char*>(header.typeTag), reinterpret_cast<const char*>(_dst + 8), 4);
  strncpy(reinterpret_cast<char*>(header.fmtTag), reinterpret_cast<const char*>(_dst + 12), 4);
  strncpy(reinterpret_cast<char*>(header.dataTag), reinterpret_cast<const char*>(_dst + 36), 4);

  header.dataLength = get_le32(_dst + 4) - 36;
  header.codec = get_le16(_dst + 20);
  header.numberOfChannels = get_le16(_dst + 22);
  header.sampleRateHz = get_le32(_dst + 24);
  header.bytesPerSecond = get_le32(_dst + 28);
  header.bytesPerSample = get_le16(_dst + 32);
  header.bitsPerSample = get_le16(_dst + 34);
  header.dataLength = get_le32(_dst + 40);
}
