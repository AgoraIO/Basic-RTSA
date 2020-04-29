//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once

#include "AgoraBase.h"
#include "media_file_parser.h"

class YuvFileParser : public VideoFileParser {
 public:
  YuvFileParser(const char* filepath, int width, int heigth,
                agora::media::VIDEO_PIXEL_FORMAT format);
  virtual ~YuvFileParser();

 public:
  // IMediaFileParser
  bool open() override;
  bool hasNext() override;

  void getNext(char* buffer, int* length) override;

 private:
  char* filePath_;
  int width_;
  int heigth_;
  int stride_;
  agora::media::VIDEO_PIXEL_FORMAT pixelFormat_;

  FILE* fileHandle_;
  int currentBufPos_;
  int dataLengthInBuffer_;
  bool isEof_;
};
