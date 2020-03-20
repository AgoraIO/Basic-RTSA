//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once
#include <stdint.h>

namespace webrtc {
enum FrameType {
  kEmptyFrame = 0,
  kAudioFrameSpeech = 1,
  kAudioFrameCN = 2,
  kVideoFrameKey = 3,
  kVideoFrameDelta = 4,
};
}

enum NaluType : uint8_t {
  kSlice = 1,
  kIdr = 5,
  kSei = 6,
  kSps = 7,
  kPps = 8,
  kAud = 9,
  kEndOfSequence = 10,
  kEndOfStream = 11,
  kFiller = 12,
  kStapA = 24,
  kFuA = 28
};

enum SliceType : uint8_t { kP = 0, kB = 1, kI = 2, kSp = 3, kSi = 4 };

const uint8_t kNaluTypeMask = 0x1F;

// The size of the NALU type byte (1).
const size_t kNaluTypeSize = 1;

inline NaluType ParseNaluType(uint8_t data) { return static_cast<NaluType>(data & kNaluTypeMask); }

struct IVF_HEADER {
  uint32_t signature;
  uint16_t version;
  uint16_t head_len;
  uint32_t codec;
  uint16_t width;
  uint16_t height;
  uint32_t time_scale;
  uint32_t frame_rate;
  uint32_t frames;
  uint32_t unused;
};

struct IVF_PAYLOAD {
  uint32_t length;
  uint32_t frame_type;
  uint64_t timestamp;
};
