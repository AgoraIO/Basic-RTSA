//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include <string.h>

#include "AgoraBase.h"
#include "yuv_file_parser.h"
#include "utils/I420_buffer.h"

static int ReadPlane(uint8_t* buf, int width, int height, int stride, FILE* file) {
  for (int i = 0; i < height; i++, buf += stride) {
    if (fread(buf, 1, width, file) != static_cast<unsigned int>(width)) return -1;
  }
  return 0;
}

YuvFileParser::YuvFileParser(const char* filepath, int width, int heigth,
    agora::media::VIDEO_PIXEL_FORMAT format)
    : filePath_(strdup(filepath)),
      width_(width),
      heigth_(heigth),
      pixelFormat_(format) {
  // only I420 is supported.
  assert(format == agora::media::VIDEO_PIXEL_FORMAT::VIDEO_PIXEL_I420);
}

YuvFileParser::~YuvFileParser() {
  if (fileHandle_) {
    fclose(fileHandle_);
  }
  free(static_cast<void*>(filePath_));
}

bool YuvFileParser::open() {
  fileHandle_ = fopen(filePath_, "rb");
  return fileHandle_ != nullptr;
}

bool YuvFileParser::hasNext() {
  return isEof_ != true;
}

void YuvFileParser::getNext(char* buffer, int* length) {
  I420Buffer* buff = reinterpret_cast<I420Buffer*>(buffer);

  int width = buff->width();
  int height = buff->height();
  int chroma_width = buff->ChromaWidth();
  int chroma_height = buff->ChromaHeight();

  int read_length = 0;
  read_length = ReadPlane(buff->MutableDataY(), width, height, buff->StrideY(), fileHandle_);

  if (read_length) {
    if (length) *length = read_length;
    read_length = ReadPlane(buff->MutableDataU(), chroma_width, chroma_height, buff->StrideU(), fileHandle_);

    if (read_length) {
      if (length) *length += read_length;
      read_length = ReadPlane(buff->MutableDataV(), chroma_width, chroma_height, buff->StrideV(), fileHandle_);

      if (read_length) {
        if (length) *length += read_length;
      } else {
        isEof_ = true;
      }
    } else {
      isEof_ = true;
    }
  } else {
    isEof_ = true;
  }
}
