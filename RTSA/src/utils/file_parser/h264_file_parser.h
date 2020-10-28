//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-08.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once

#include <stdio.h>
#include "media_file_parser.h"

class H264FileParser : public VideoFileParser {
 public:
  explicit H264FileParser(const char* filepath);
  virtual ~H264FileParser();

  bool open() override;
  bool hasNext() override;
  void getNext(char* buffer, int* length) override;

 private:
  void readData();

 private:
  static constexpr int BufferSize = 409600;

  char* filePath_;
  FILE* fileHandle_;
  unsigned char dataBuffer_[BufferSize] = {0};
  bool isEof_;
  int currentBytePos_;
  int dataEndPos_;
  int currentFrameStart_;

  int readsize_;
};
