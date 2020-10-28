//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//
#include "I420_buffer.h"
#include "aligned_alloc.h"
#include "wrapper/utils.h"
#include <cstring>

// Aligning pointer to 64 bytes for improved performance, e.g. use SIMD.
static const int kBufferAlignment = 64;

static int I420DataSize(int height, int stride_y, int stride_u, int stride_v) {
  return stride_y * height + (stride_u + stride_v) * ((height + 1) / 2);
}

I420Buffer::I420Buffer(int width, int height)
    : I420Buffer(width, height, width, (width + 1) / 2, (width + 1) / 2) {}

I420Buffer::I420Buffer(int width,
                       int height,
                       int stride_y,
                       int stride_u,
                       int stride_v)
    : width_(width),
      height_(height),
      stride_y_(stride_y),
      stride_u_(stride_u),
      stride_v_(stride_v),
      data_(static_cast<uint8_t*>(
          AlignedMalloc(I420DataSize(height, stride_y, stride_u, stride_v),
                        kBufferAlignment))) {
  assert(width > 0);
  assert(height > 0);
  assert(stride_y >= width);
  assert(stride_u >= (width + 1) / 2);
  assert(stride_v >= (width + 1) / 2);
}

I420Buffer::~I420Buffer() {
    if (data_) AlignedFree(data_.get());
}

// static
I420Buffer* I420Buffer::Create(int width, int height) {
  return new I420Buffer(width, height);
}

// static
I420Buffer* I420Buffer::Create(int width,
                               int height,
                               int stride_y,
                               int stride_u,
                               int stride_v) {
  return new I420Buffer(width, height, stride_y, stride_u, stride_v);
}

void I420Buffer::InitializeData() {
  memset(data_.get(), 0,
         I420DataSize(height_, stride_y_, stride_u_, stride_v_));
}

int I420Buffer::width() const {
  return width_;
}

int I420Buffer::height() const {
  return height_;
}

int I420Buffer::ChromaWidth() const {
  return (width() + 1) / 2;
}

int I420Buffer::ChromaHeight() const {
  return (height() + 1) / 2;
}

const uint8_t* I420Buffer::Data() const {
  return data_.get();
}

const uint8_t* I420Buffer::DataY() const {
  return data_.get();
}

const uint8_t* I420Buffer::DataU() const {
  return data_.get() + stride_y_ * height_;
}

const uint8_t* I420Buffer::DataV() const {
  return data_.get() + stride_y_ * height_ + stride_u_ * ((height_ + 1) / 2);
}

int I420Buffer::StrideY() const {
  return stride_y_;
}

int I420Buffer::StrideU() const {
  return stride_u_;
}

int I420Buffer::StrideV() const {
  return stride_v_;
}

uint8_t* I420Buffer::MutableDataY() {
  return const_cast<uint8_t*>(DataY());
}
uint8_t* I420Buffer::MutableDataU() {
  return const_cast<uint8_t*>(DataU());
}
uint8_t* I420Buffer::MutableDataV() {
  return const_cast<uint8_t*>(DataV());
}

