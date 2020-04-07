//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include "media_packet_sender.h"

#include <stdint.h>
#include <chrono>
#include <thread>
#include <cstring>

#include "connection_wrapper.h"
#include "local_user_wrapper.h"
#include "utils.h"

MediaPacketSender::MediaPacketSender(const SendConfig& config, int uid)
    : config_(config), control_packet_sender_(nullptr), userId_(uid) {}

MediaPacketSender::~MediaPacketSender() = default;

bool MediaPacketSender::initialize(agora::base::IAgoraService* service,
                                   agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                                   std::shared_ptr<ConnectionWrapper> connection) {
  media_packet_sender_ = factory->createMediaPacketSender();
  if (!media_packet_sender_) {
    return false;
  }

  if (config_.audioTest) {
    auto audioTrack_ = service->createCustomAudioTrack(media_packet_sender_);
    audioTrack_->setEnabled(true);
    connection->GetLocalUser()->PublishAudioTrack(audioTrack_);
  } else {
    auto videoTrack_ = service->createCustomVideoTrack(media_packet_sender_);
    videoTrack_->setEnabled(true);
    connection->GetLocalUser()->PublishVideoTrack(videoTrack_);
  }
  control_packet_sender_ =
      connection->GetLocalUser()->GetLocalUser()->getMediaControlPacketSender();
  return true;
}

void MediaPacketSender::sendPackets() {
  if (config_.lengthPerSend > config_.testDataLength) {
    printf("Illegal Args: total length(%d) < length per send(%d)\n", config_.testDataLength,
           config_.lengthPerSend);
    return;
  }
  int* sentNumPacketsPtr = (config_.audioTest ? &sent_audio_packets_ : &sent_video_packets_);
  int* sentNumControlPacketsPtr =
      (config_.audioTest ? &sent_audio_control_packets_ : &sent_video_control_packets_);
  int oldSentNumPackets = *sentNumPacketsPtr;

  std::unique_ptr<uint8_t[]> data_buffer(new uint8_t[config_.lengthPerSend + 4]);
  std::unique_ptr<uint8_t[]> control_data_buffer(new uint8_t[config_.lengthPerSend + 4]);
  auto start_time = now_ms();
  int sent_bytes = 0, sent_control_bytes = 0;
  while (sent_bytes < config_.testDataLength) {
    int remaining_bytes = config_.testDataLength - sent_bytes;
    int send_length =
        remaining_bytes > config_.lengthPerSend ? config_.lengthPerSend : remaining_bytes;

    agora::media::PacketOptions options;
    options.time_stamp = now_ms();

    data_buffer[0] = (sent_bytes & 0xFF);
    data_buffer[1] = ((sent_bytes >> 8) & 0xFF);
    data_buffer[2] = ((sent_bytes >> 16) & 0xFF);
    data_buffer[3] = ((sent_bytes >> 24) & 0xFF);
    // Audio: 0x01 for data, 0x02 for control, 0x03 for broadcast control
    // Video: 0x04 for data, 0x05 for control, 0x06 for broadcast control
    memset(data_buffer.get() + 4, (config_.audioTest ? 0x01 : 0x04), send_length);

    media_packet_sender_->sendMediaPacket(data_buffer.get(), send_length + 4, options);
    sent_bytes += send_length;

    if ((*sentNumPacketsPtr) % 10 == 0 && control_packet_sender_) {
      int send_control_length = config_.lengthPerSend / 2;
      control_data_buffer[0] = (sent_control_bytes & 0xFF);
      control_data_buffer[1] = ((sent_control_bytes >> 8) & 0xFF);
      control_data_buffer[2] = ((sent_control_bytes >> 16) & 0xFF);
      control_data_buffer[3] = ((sent_control_bytes >> 24) & 0xFF);

      if ((*sentNumPacketsPtr) % 20 == 0) {
        char buf[16] = {0};
        snprintf(buf, sizeof(buf), "%d", userId_ + 3);
        memset(control_data_buffer.get() + 4, (config_.audioTest ? 0x02 : 0x05),
               send_control_length);
        control_packet_sender_->sendPeerMediaControlPacket(buf, control_data_buffer.get(),
                                                           send_control_length + 4);
      } else {
        memset(control_data_buffer.get() + 4, (config_.audioTest ? 0x03 : 0x06),
               send_control_length);
        control_packet_sender_->sendBroadcastMediaControlPacket(control_data_buffer.get(),
                                                                send_control_length + 4);
      }

      sent_control_bytes += send_control_length;
      *sentNumControlPacketsPtr = *sentNumControlPacketsPtr + 1;
    }

    *sentNumPacketsPtr = *sentNumPacketsPtr + 1;

    uint64_t send_frame_end = now_ms();
    uint64_t next_time = start_time +
                         ((*sentNumPacketsPtr - oldSentNumPackets) * config_.sendIntervalMs) +
                         config_.sendIntervalMs;
    if (next_time > send_frame_end) {
      std::this_thread::sleep_for(std::chrono::milliseconds(next_time - send_frame_end));
    }
  }
}
