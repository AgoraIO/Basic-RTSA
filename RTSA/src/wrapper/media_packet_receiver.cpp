//  Agora RTC/MEDIA SDK
//
//  Created by minbo in 2019/9/23.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#include "media_packet_receiver.h"

#include <unistd.h>

#include "audio_pcm_frame_handler.h"
#include "connection_wrapper.h"
#include "local_user_wrapper.h"

#include "utils.h"

MediaPacketReceiver::MediaPacketReceiver() = default;

MediaPacketReceiver::~MediaPacketReceiver() = default;

void MediaPacketReceiver::SetVerbose(bool verbose){
  verbose_ = verbose;
}

bool MediaPacketReceiver::onMediaPacketReceived(const uint8_t* packet, size_t length) {
  int position = static_cast<int>(packet[0]) | (static_cast<int>(packet[1]) << 8) |
                 (static_cast<int>(packet[2]) << 16) | (static_cast<int>(packet[3]) << 24);
  bool valid = true, audioTest = true;

  switch (*(packet + 4)) {
    case 0x01:
      break;
    case 0x04:
      audioTest = false;
      break;
    default:
      AGO_LOG("Media Packet Received fail: invalid format\n");
      return false;
  }
  int i = 4;
  while (i < length) {
    valid = valid && (*(packet + i) == (audioTest ? 0x01 : 0x04));
    i++;
  }

  if (!valid) AGO_LOG("Media Packet Fail: invalid data ...\n");

  received_media_packet_bytes_ += length;

  if (verbose_) {
    if (audioTest) {
      AGO_LOG("Audio Media Packet Recv: position -> %d, length -> %zu\n", position, length);
    } else {
      AGO_LOG("Video Media Packet Recv: position -> %d, length -> %zu\n", position, length);
    }
  }

  return true;
}

bool MediaPacketReceiver::onMediaControlPacketReceived(const uint8_t* packet, size_t length) {
  int position = static_cast<int>(packet[0]) | (static_cast<int>(packet[1]) << 8) |
                 (static_cast<int>(packet[2]) << 16) | (static_cast<int>(packet[3]) << 24);
  bool valid = true;
  int controlType;
  switch (controlType = *(packet + 4)) {
    case 0x02:
    case 0x03:
    case 0x05:
    case 0x06:
      break;
    default:
      AGO_LOG("Media Control Packet Received fail: invalid format\n");
      return false;
  }
  int i = 4;
  while (i < length) {
    valid = valid && (*(packet + i) == controlType);
    i++;
  }

  if (!valid) AGO_LOG("Media Control Packet Fail: invalid data ...\n");
  if (verbose_) {
    switch (controlType) {
    case 0x02:
      printf("Audio Media Control Packet Recv: position -> %d, length -> %zu\n", position, length);
      break;
    case 0x03:
      printf("Audio Media Broadcast Control Packet Recv: position -> %d, length -> %zu\n", position, length);
      break;
    case 0x05:
      printf("Video Media Control Packet Recv: position -> %d, length -> %zu\n", position, length);
      break;
    case 0x06:
      printf("Video Media Broadcast Control Packet Recv: position -> %d, length -> %zu\n", position, length);
      break;
    }
  }
  received_control_packet_bytes_ += length;

  return true;
}
