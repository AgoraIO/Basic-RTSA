//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once

#include "AgoraBase.h"

class IMediaFileParser {
 public:
  virtual ~IMediaFileParser() {}
  virtual bool open() = 0;
  virtual bool hasNext() = 0;
  virtual void getNext(char* buffer, int* length) = 0;
};

class AudioFileParser : public IMediaFileParser {
 public:
  virtual ~AudioFileParser();
  virtual agora::rtc::AUDIO_CODEC_TYPE getCodecType() = 0;
  virtual int getSampleRateHz() = 0;
  virtual int getNumberOfChannels() = 0;
  virtual int getBitsPerSample() { return 0; }
  virtual int reset() { return 0; }
};

class VideoFileParser : public IMediaFileParser {
 public:
  virtual ~VideoFileParser() {}
};
