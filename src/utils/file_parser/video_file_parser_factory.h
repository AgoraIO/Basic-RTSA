//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once
#include <stdint.h>
#include <memory>

#include "AgoraBase.h"
#include "media_file_parser.h"

enum class VIDEO_FILE_TYPE : uint8_t {
  VIDEO_FILE_H264 = 1,
  VIDEO_FILE_YUV = 2
};

class VideoFileParserFactory {
 public:
  struct ParserConfig {
    VIDEO_FILE_TYPE fileType{VIDEO_FILE_TYPE::VIDEO_FILE_H264};
    const char* filePath{nullptr};
    int width{0};
    int height{0};
    agora::media::VIDEO_PIXEL_FORMAT pixelFormat{agora::media::VIDEO_PIXEL_I420};
  };

 public:
  static VideoFileParserFactory& Instance();
  ~VideoFileParserFactory();

  std::unique_ptr<VideoFileParser> createVideoFileParser(ParserConfig& config);

 private:
  std::unique_ptr<VideoFileParser> createH264FileParser(const char* filepath);
  std::unique_ptr<VideoFileParser> createYuvFileParser(const char* filepath, int width,
      int heigth, agora::media::VIDEO_PIXEL_FORMAT format);

 private:
  VideoFileParserFactory();
};
