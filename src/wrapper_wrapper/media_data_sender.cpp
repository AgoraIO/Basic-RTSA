//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#include "media_data_sender.h"

#include <string.h>
#include <chrono>
#include <thread>

#include "utils/file_parser/fixed_frame_length_audio_file_parser.h"
#include "wrapper/audio_frame_sender.h"
#include "wrapper/connection_wrapper.h"
#include "wrapper/local_user_wrapper.h"
#include "wrapper/media_packet_sender.h"
#include "wrapper/statistic_dump.h"
#include "wrapper/utils.h"
#include "wrapper/video_frame_sender.h"

MediaDataSender::MediaDataSender(agora::base::IAgoraService* service, int uid)
    : uid_(uid), statisticPacketNum_(0), service_(service) {
  pid_ = getpid();
  tid_ = gettid();
  factory_ = service_->createMediaNodeFactory();
}

MediaDataSender::~MediaDataSender() {
  connection_->Disconnect();
}

void MediaDataSender::setVerbose(bool verbose) { verbose_ = verbose; }

bool MediaDataSender::connect(const char* channelId, agora::user_id_t userId) {
  ConnectionConfig config;
  config.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
  config.channelProfile = agora::CHANNEL_PROFILE_LIVE_BROADCASTING;

  connection_ = ConnectionWrapper::CreateConnection(service_, config);
  if (!connection_->Connect(API_CALL_APPID, channelId, userId)) {
    return false;
  }
  return true;
}

void MediaDataSender::sendEncodedAudioFile(const char* filepath, AUDIO_FILE_TYPE filetype) {
  auto frame_sender = std::make_shared<EncodedAudioFrameSender>(filepath, filetype);
  if (!frame_sender->initialize(service_, factory_, connection_)) {
    printf("Initialize test file %s for sending successfully\n", filepath);
    return;
  }
  frame_sender->sendAudioFrames();
}

void MediaDataSender::sendAudioAACFile(const char* filepath, bool heaac) {
  if (heaac) {
    sendEncodedAudioFile(filepath, AUDIO_FILE_TYPE::AUDIO_FILE_HEAAC);
  } else {
    sendEncodedAudioFile(filepath, AUDIO_FILE_TYPE::AUDIO_FILE_AACLC);
  }
}

void MediaDataSender::sendAudioOpusFile(const char* filepath) {
  sendEncodedAudioFile(filepath, AUDIO_FILE_TYPE::AUDIO_FILE_OPUS);
}

void MediaDataSender::sendAudioPcmFile(const char* filepath) {
  auto frame_sender = std::make_shared<AudioPcmFrameSender>(filepath);
  if (!frame_sender->initialize(service_, factory_, connection_)) {
    printf("Initialize test file %s for sending successfully\n", filepath);
    return;
  }
  printf("Open test file %s successfully\n", filepath);
  frame_sender->sendAudioFrames();
}

void MediaDataSender::sendAudioMediaPacket() {
  printf("Start to send audio media packet ...\n");
  SendConfig args;
  args.testDataLength = 500000;
  args.lengthPerSend = 350;
  args.sendIntervalMs = 7;
  args.audioTest = true;

  std::unique_ptr<MediaPacketSender> packet_sender(new MediaPacketSender(args, uid_));
  packet_sender->initialize(service_, factory_, connection_);
  packet_sender->sendPackets();
}

void MediaDataSender::sendVideoVp8File(const char* filepath) {
  std::unique_ptr<VideoVP8FrameSender> video_frame_sender(new VideoVP8FrameSender(filepath));
  video_frame_sender->initialize(service_, factory_, connection_);
  video_frame_sender->sendVideoFrames();
}

void MediaDataSender::sendVideoH264File(const char* filepath) {
  std::unique_ptr<VideoH264FileSender> video_frame_sender(new VideoH264FileSender(filepath));
  video_frame_sender->initialize(service_, factory_, connection_);
  video_frame_sender->sendVideoFrames();
}

void MediaDataSender::sendVideo() {
  std::unique_ptr<VideoH264FramesSender> video_frame_sender(new VideoH264FramesSender());
  video_frame_sender->initialize(service_, factory_, connection_);
  video_frame_sender->sendVideoFrames();

  sentNumVideoFrames_ = video_frame_sender->getSentFrameNum();
  if (verbose_) {
    int sentBytes = video_frame_sender->getSentBytes();
    AGO_LOG("Send video %d bytes", sentBytes);
  }
}

void MediaDataSender::sendVideoMediaPacket() {
  SendConfig args;
  args.testDataLength = 1500000;
  args.lengthPerSend = 1250;
  args.sendIntervalMs = 7;
  args.audioTest = false;
  std::unique_ptr<MediaPacketSender> packet_sender(new MediaPacketSender(args, uid_));
  packet_sender->initialize(service_, factory_, connection_);
  packet_sender->sendPackets();
}
