//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-08.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#include "h264_file_parser.h"

#include <stdlib.h>
#include <string.h>
#include <memory>

H264FileParser::H264FileParser(const char* filepath)
    : filePath_(strdup(filepath)),
      fileHandle_(nullptr),
      isEof_(false),
      currentBytePos_(0),
      dataEndPos_(0),
      currentFrameStart_(0),
      readsize_(0) {}

H264FileParser::~H264FileParser() {
  if (fileHandle_) {
    fclose(fileHandle_);
  }
  free(static_cast<void*>(filePath_));
}

bool H264FileParser::open() {
  fileHandle_ = fopen(filePath_, "r");
  return fileHandle_ != nullptr;
}

bool H264FileParser::hasNext() { return (!isEof_) || (currentBytePos_ < dataEndPos_); }

void H264FileParser::getNext(char* buffer, int* length) {
  readData();
  while (currentBytePos_ < dataEndPos_ - 2) {
    if (dataBuffer_[currentBytePos_ + 2] > 1) {
      currentBytePos_ += 3;
    } else if (dataBuffer_[currentBytePos_ + 2] == 1 && dataBuffer_[currentBytePos_ + 1] == 0 &&
               dataBuffer_[currentBytePos_] == 0) {
      if ((*length) < currentBytePos_ - currentFrameStart_) {
        *length = 0;
        return;
      }
      if (dataBuffer_[currentBytePos_ - 1] == 0) {
        *length = currentBytePos_ - 1 - currentFrameStart_;
      } else {
        *length = currentBytePos_ - currentFrameStart_;
      }
      memcpy(buffer, dataBuffer_ + currentFrameStart_, *length);

      if (dataBuffer_[currentBytePos_ - 1] == 0) {
        currentFrameStart_ = currentBytePos_ - 1;
      } else {
        currentFrameStart_ = currentBytePos_;
      }
      currentBytePos_ += 3;
      return;
    } else {
      ++currentBytePos_;
    }
  }
  if (isEof_ && currentBytePos_ >= (dataEndPos_ - 3)) {
    if ((*length) < dataEndPos_ - currentFrameStart_) {
      *length = 0;
      return;
    }
    *length = dataEndPos_ - currentFrameStart_;
    memcpy(buffer, dataBuffer_ + currentFrameStart_, *length);
    currentBytePos_ = dataEndPos_;
  }
}

void H264FileParser::readData() {
  if (isEof_) {
    return;
  }
  if (dataEndPos_ > 0 && currentFrameStart_ > 0) {
    memcpy(dataBuffer_, dataBuffer_ + currentFrameStart_, dataEndPos_ - currentFrameStart_);
    dataEndPos_ = dataEndPos_ - currentFrameStart_;
    currentFrameStart_ = 0;
    currentBytePos_ = 4;
  }

  if (dataEndPos_ == 0) {
    currentBytePos_ = 4;
  }

  int buferRemainingSize = BufferSize - dataEndPos_;
  while (!isEof_ && buferRemainingSize > 0) {
    size_t readsize = fread(dataBuffer_ + dataEndPos_, 1, buferRemainingSize, fileHandle_);
    if (readsize <= 0) {
      isEof_ = true;
      continue;
    }
    readsize_ += readsize;
    dataEndPos_ += readsize;
    buferRemainingSize = BufferSize - dataEndPos_;
  }
}
