//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//
#pragma once

#include <stdint.h>
#include <memory>

class I420Buffer {
 public:
  static I420Buffer* Create(int width, int height);
  static I420Buffer* Create(int width, int height,
                            int stride_y, int stride_u,
                            int stride_v);
  static void Release(I420Buffer* buffer) { delete buffer; }

 public:

  // Sets all three planes to all zeros. Used to work around for
  // quirks in memory checkers
  // (https://bugs.chromium.org/p/libyuv/issues/detail?id=377) and
  // ffmpeg (http://crbug.com/390941).
  // TODO(nisse): Deprecated. Should be deleted if/when those issues
  // are resolved in a better way. Or in the mean time, use SetBlack.
  void InitializeData();

  int width() const;
  int height() const;
  int ChromaWidth() const;
  int ChromaHeight() const;

  const uint8_t* Data() const;
  const uint8_t* DataY() const;
  const uint8_t* DataU() const;
  const uint8_t* DataV() const;

  int StrideY() const;
  int StrideU() const;
  int StrideV() const;

  uint8_t* MutableDataY();
  uint8_t* MutableDataU();
  uint8_t* MutableDataV();

 protected:
  I420Buffer(int width, int height);
  I420Buffer(int width, int height, int stride_y, int stride_u, int stride_v);

  ~I420Buffer();

 private:
  const int width_;
  const int height_;
  const int stride_y_;
  const int stride_u_;
  const int stride_v_;
  const std::unique_ptr<uint8_t> data_;
};