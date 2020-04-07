//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include "video_frame_sender.h"

#include <stdio.h>
#include <thread>
#include <cstring>

#include "test_data/foreman_frames.h"
#include "connection_wrapper.h"
#include "local_user_wrapper.h"
#include "utils.h"
#include "utils/bitbuffer.h"
#include "utils/file_parser/h264_file_parser.h"
#include "video_frame_sender_internal.h"

VideoFrameSender::VideoFrameSender() = default;

VideoFrameSender::~VideoFrameSender() = default;

VideoVP8FrameSender::VideoVP8FrameSender(const char* filepath) : file_path_(filepath) {}

VideoVP8FrameSender::~VideoVP8FrameSender() = default;

bool VideoVP8FrameSender::initialize(agora::base::IAgoraService* service,
                                     agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                                     std::shared_ptr<ConnectionWrapper> connection) {
  video_encoded_image_sender_ = factory->createVideoEncodedImageSender();
  if (!video_encoded_image_sender_) {
    return false;
  }
  auto customVideoTrack = service->createCustomVideoTrack(video_encoded_image_sender_);
  agora::rtc::VideoEncoderConfiguration encoder_config;
  encoder_config.codecType = agora::rtc::VIDEO_CODEC_VP8;
  encoder_config.bitrate = 1 * 1000 * 1000;
  encoder_config.minBitrate = 500 * 1000;
  encoder_config.dimensions.width = 2560;
  encoder_config.dimensions.height = 1440;
  encoder_config.frameRate = 30;
  customVideoTrack->setVideoEncoderConfiguration(encoder_config);
  connection->GetLocalUser()->PublishVideoTrack(customVideoTrack);

  return true;
}

void VideoVP8FrameSender::sendVideoFrames() {
  const int loop_time_ms = -1;
  const char* test_file = file_path_.c_str();
  FILE* f = fopen(test_file, "rb");
  IVF_HEADER header = {0};
  uint64_t last_time_diff = 0;
  uint64_t last_time_stamp = 0;
  agora::rtc::VIDEO_FRAME_TYPE frame_type;
  agora::rtc::VIDEO_CODEC_TYPE codec;

#define CONVERT_TO_INT(a, b, c, d) \
    ((ENDIANNESS == 'l') ? (a | b << 8 | c << 16 | d << 24) : (a << 24 | b << 16 | c << 8 | d))

  fread(&header, sizeof(header), 1, f);
  if (header.codec == CONVERT_TO_INT('V', 'P', '8', '0')) {
    codec = agora::rtc::VIDEO_CODEC_VP8;
  } else if (header.codec == CONVERT_TO_INT('V', 'P', '9', '0')) {
    codec = agora::rtc::VIDEO_CODEC_VP9;
  } else if (header.codec == CONVERT_TO_INT('H', '2', '6', '4')) {
    codec = agora::rtc::VIDEO_CODEC_H264;
  } else {
    codec = agora::rtc::VIDEO_CODEC_VP8;
  }
  AGO_LOG("Begin to send ivf file, width %d, height %d, frame_rate %d, time_scale %d, frames %d",
          header.width, header.height, header.frame_rate, header.time_scale, header.frames);
  fseek(f, header.head_len, SEEK_SET);
  auto start_time = now_ms();
  while (true) {
    auto overhead_begin = now_ms();
    if ((loop_time_ms != -1) && (overhead_begin - start_time) >= loop_time_ms) break;
    IVF_PAYLOAD payload = {0};
    fread(&payload, sizeof(payload), 1, f);
    if (payload.length == 0) {
      fseek(f, header.head_len, SEEK_SET);
      last_time_stamp = 0;
      continue;
    }
    char* buf = new char[payload.length];
    fread(buf, payload.length, 1, f);
    agora::rtc::VIDEO_FRAME_TYPE frame_type;
    if (payload.frame_type == webrtc::kVideoFrameKey) {
      frame_type = agora::rtc::VIDEO_FRAME_TYPE_KEY_FRAME;
    } else if (payload.frame_type == webrtc::kVideoFrameDelta) {
      frame_type = agora::rtc::VIDEO_FRAME_TYPE_DELTA_FRAME;
    } else {
      frame_type = agora::rtc::VIDEO_FRAME_TYPE_UNKNOW;
    }
    uint64_t wait_time_ms =
        (last_time_stamp == 0
             ? last_time_diff
             : ((payload.timestamp - last_time_stamp) / (header.time_scale / 1000)));
    last_time_diff = wait_time_ms;
    last_time_stamp = payload.timestamp;
    auto overhead = now_ms() - overhead_begin;
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms - overhead));

    agora::rtc::EncodedVideoFrameInfo videoEncodedFrameInfo;
    videoEncodedFrameInfo.frameType = frame_type;
    videoEncodedFrameInfo.width = header.width;
    videoEncodedFrameInfo.height = header.height;
    videoEncodedFrameInfo.rotation = agora::rtc::VIDEO_ORIENTATION_0;
    videoEncodedFrameInfo.codecType = codec;
    video_encoded_image_sender_->sendEncodedVideoImage(reinterpret_cast<uint8_t*>(buf),
                                                       payload.length, videoEncodedFrameInfo);
    delete[] buf;
  }
  fclose(f);
}

std::vector<uint8_t> ParseRbsp(const uint8_t* data, size_t length) {
  std::vector<uint8_t> out;
  out.reserve(length);

  for (size_t i = 0; i < length;) {
    // Be careful about over/underflow here. byte_length_ - 3 can underflow, and
    // i + 3 can overflow, but byte_length_ - i can't, because i < byte_length_
    // above, and that expression will produce the number of bytes left in
    // the stream including the byte at i.
    if (length - i >= 3 && !data[i] && !data[i + 1] && data[i + 2] == 3) {
      // Two rbsp bytes.
      out.push_back(data[i++]);
      out.push_back(data[i++]);
      // Skip the emulation byte.
      i++;
    } else {
      // Single rbsp byte.
      out.push_back(data[i++]);
    }
  }
  return out;
}

VideoH264FileSender::VideoH264FileSender(const char* filepath) : file_path_(filepath) {}

VideoH264FileSender::~VideoH264FileSender() = default;

bool VideoH264FileSender::initialize(agora::base::IAgoraService* service,
                                     agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                                     std::shared_ptr<ConnectionWrapper> connection) {
  video_encoded_image_sender_ = factory->createVideoEncodedImageSender();
  if (!video_encoded_image_sender_) {
    return false;
  }
  auto customVideoTrack =
      service->createCustomVideoTrack(video_encoded_image_sender_, false, agora::base::CC_DISABLED);
  connection->GetLocalUser()->PublishVideoTrack(customVideoTrack);

  std::unique_ptr<H264FileParser> parser(new H264FileParser(file_path_.c_str()));
  bool ret = parser->open();
  if (!ret) {
    printf("Open test file %s failed\n", file_path_.c_str());
    return false;
  }
  printf("Open test file %s successfully\n", file_path_.c_str());
  file_parser_ = std::move(parser);
  return true;
}

void VideoH264FileSender::sendVideoFrames() {
  std::unique_ptr<char[]> buffer(new char[40960]);
  std::unique_ptr<char[]> sliceBuffer(new char[40960]);
  int totalLength = 0;
  int totalSendLenth = 0;
  int length = 40960;
  int i = 0;
  int bufdatalen = 0;
  uint32_t last_slice_type;
  NaluType lastNaluType = kSei;
  int frameNum = 0;
  while (file_parser_->hasNext()) {
    length = 40960;
    file_parser_->getNext(sliceBuffer.get(), &length);
    NaluType naluType = ParseNaluType(sliceBuffer[4]);

    if (naluType == NaluType::kSlice || naluType == NaluType::kIdr) {
      std::vector<uint8_t> unpacked_buffer =
          ParseRbsp(reinterpret_cast<const unsigned char*>(sliceBuffer.get() + 4), length - 4);
      BitBuffer slice_reader(unpacked_buffer.data() + kNaluTypeSize,
                             unpacked_buffer.size() - kNaluTypeSize);
      // first_mb_in_slice
      uint32_t first_mb_in_slice;
      slice_reader.ReadExponentialGolomb(&first_mb_in_slice);

      // slice_type: ue(v)
      uint32_t slice_type;
      slice_reader.ReadExponentialGolomb(&slice_type);
      slice_type %= 5;

      // New video frame found, so to send last video frame.
      if (first_mb_in_slice == 0 &&
          (lastNaluType == NaluType::kSlice || lastNaluType == NaluType::kIdr)) {
        agora::rtc::EncodedVideoFrameInfo videoEncodedFrameInfo;

        videoEncodedFrameInfo.rotation = agora::rtc::VIDEO_ORIENTATION_0;
        videoEncodedFrameInfo.codecType = agora::rtc::VIDEO_CODEC_H264;
        videoEncodedFrameInfo.framesPerSecond = 30;
        if (i % 3 != 0) {
          std::this_thread::sleep_for(std::chrono::milliseconds(34));
        } else {
          std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
        if (last_slice_type == SliceType::kI) {
          videoEncodedFrameInfo.frameType = agora::rtc::VIDEO_FRAME_TYPE_KEY_FRAME;
        } else {
          videoEncodedFrameInfo.frameType = agora::rtc::VIDEO_FRAME_TYPE_DELTA_FRAME;
        }
        video_encoded_image_sender_->sendEncodedVideoImage(reinterpret_cast<uint8_t*>(buffer.get()),
                                                           bufdatalen, videoEncodedFrameInfo);
        totalSendLenth += bufdatalen;

        //        printf(
        //            "Send %d length %d, Nalu type %d, first_mb_in_slice %u, Slice type %u, nalu
        //            number "
        //            "%d\n",
        //            frameNum++, bufdatalen, naluType, first_mb_in_slice, slice_type, i);
        i = 0;

        memcpy(buffer.get(), sliceBuffer.get(), length);
        bufdatalen = length;
      } else {
        memcpy(buffer.get() + bufdatalen, sliceBuffer.get(), length);
        bufdatalen += length;
        ++i;
      }
      last_slice_type = slice_type;
    } else {
      memcpy(buffer.get() + bufdatalen, sliceBuffer.get(), length);
      bufdatalen += length;
      ++i;
    }
    totalLength += length;
    lastNaluType = naluType;
  }
  agora::rtc::EncodedVideoFrameInfo videoEncodedFrameInfo;

  videoEncodedFrameInfo.rotation = agora::rtc::VIDEO_ORIENTATION_0;
  videoEncodedFrameInfo.codecType = agora::rtc::VIDEO_CODEC_H264;
  videoEncodedFrameInfo.framesPerSecond = 30;
  videoEncodedFrameInfo.packetizationMode = agora::rtc::NonInterleaved;
  if (i % 3 != 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(34));
  } else {
    std::this_thread::sleep_for(std::chrono::milliseconds(33));
  }
  if (last_slice_type == SliceType::kI) {
    videoEncodedFrameInfo.frameType = agora::rtc::VIDEO_FRAME_TYPE_KEY_FRAME;
  } else {
    videoEncodedFrameInfo.frameType = agora::rtc::VIDEO_FRAME_TYPE_DELTA_FRAME;
  }
  video_encoded_image_sender_->sendEncodedVideoImage(reinterpret_cast<uint8_t*>(buffer.get()),
                                                     bufdatalen, videoEncodedFrameInfo);

  AGO_LOG("Total read length %d, total send lenth %d\n", totalLength, totalSendLenth);
}

struct VideoPacket {
  VideoPacket() : data(nullptr), size(0), flags(0), timestamp(0) {}
  uint8_t* data;
  int size;
  int flags;
  int64_t timestamp;  // ms
};

VideoH264FramesSender::VideoH264FramesSender() = default;

VideoH264FramesSender::~VideoH264FramesSender() = default;

bool VideoH264FramesSender::initialize(agora::base::IAgoraService* service,
                                       agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                                       std::shared_ptr<ConnectionWrapper> connection) {
  video_encoded_image_sender_ = factory->createVideoEncodedImageSender();
  if (!video_encoded_image_sender_) {
    return false;
  }
  agora::agora_refptr<agora::rtc::ILocalVideoTrack> customVideoTrack =
      service->createCustomVideoTrack(video_encoded_image_sender_, false, agora::base::CC_DISABLED);
  connection->GetLocalUser()->PublishVideoTrack(customVideoTrack);

  return true;
}

bool VideoH264FramesSender::sendOneFrame(struct VideoPacket* videoPacket) {
  const uint8_t* payload_data = videoPacket->data;
  size_t payload_size = videoPacket->size;

  agora::rtc::EncodedVideoFrameInfo videoEncodedFrameInfo;
  videoEncodedFrameInfo.codecType = agora::rtc::VIDEO_CODEC_H264;
  videoEncodedFrameInfo.framesPerSecond = 15;
  videoEncodedFrameInfo.frameType = videoPacket->flags & 0x1
                                        ? agora::rtc::VIDEO_FRAME_TYPE_KEY_FRAME
                                        : agora::rtc::VIDEO_FRAME_TYPE_DELTA_FRAME;

  return video_encoded_image_sender_->sendEncodedVideoImage(payload_data, payload_size,
                                                            videoEncodedFrameInfo);
}

void VideoH264FramesSender::sendVideoFrames() {
  struct VideoPacket videoPacket;
  int numFrames = sizeof(foreman_frames) / sizeof(foreman_frames[0]);
  for (int i = 0; i < numFrames; ++i) {
    if (i % 3 != 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(67));
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(66));
    }
    videoPacket.data = foreman_frames[i].frame_data;
    videoPacket.size = foreman_frames[i].frame_len;
    if (i % 30 == 0) {
      videoPacket.flags = 1;
    } else {
      videoPacket.flags = 0;
    }

    if (i % 3 != 0) {
      videoPacket.timestamp += 67;
    } else {
      videoPacket.timestamp += 66;
    }
    sendBytes_ += foreman_frames[i].frame_len;
    ++sendNumFrames_;
    if (!sendOneFrame(&videoPacket)) {
      AGO_LOG("Send video stream failed\n");
      break;
    }
  }
}
