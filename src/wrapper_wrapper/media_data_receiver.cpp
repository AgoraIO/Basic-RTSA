//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#include "media_data_receiver.h"

#include <functional>
#include <thread>

#include "api2/IAgoraService.h"
#include "api2/NGIAgoraLocalUser.h"

#include "wrapper/audio_frame_observer.h"
#include "wrapper/audio_pcm_frame_handler.h"
#include "wrapper/connection_wrapper.h"
#include "wrapper/local_user_wrapper.h"
#include "wrapper/media_packet_receiver.h"
#include "wrapper/statistic_dump.h"
#include "wrapper/utils.h"
#include "wrapper/video_encoded_frame_receiver.h"

MediaDataReceiver::MediaDataReceiver(agora::base::IAgoraService* service, const MediaDataRecvConfig &config)
    : service_(service),
      config_(config),
      pid_(0),
      tid_(0),
      statisticPacketNum_(0),
      pullerTread_(nullptr) {
  pid_ = getpid();
  tid_ = 0;
}

MediaDataReceiver::~MediaDataReceiver() {
  connection_->Disconnect();
}

void MediaDataReceiver::setVerbose(bool verbose) { verbose_ = verbose; }

void MediaDataReceiver::setupPuller() {
  audioPuller_ = createAudioPcmPuller(connection_->GetLocalUser()->GetLocalUser(),
      config_.audio_data_pull_param, config_.duration);

  pullerTread_ = std::make_shared<std::thread>(
      std::bind(&AudioPCMPuller::startPullAudioPCM, audioPuller_.get()));
}

void MediaDataReceiver::setMediaPacketReceiver() {
  media_packet_receiver_.reset(new MediaPacketReceiver);
  media_packet_receiver_->SetVerbose(true); // enable the log.
  connection_->GetLocalUser()->GetLocalUser()->registerMediaControlPacketReceiver(
      media_packet_receiver_.get());

  connection_->GetLocalUser()->setMediaPacketReceiver(media_packet_receiver_.get());
}

void MediaDataReceiver::setupVideoReceiving() {
  encodedFrameReceiver_.reset(new VideoEncodedFrameReceiver);
  connection_->GetLocalUser()->setVideoEncodedImageReceiver(encodedFrameReceiver_.get());
}

bool MediaDataReceiver::connect(const char* channelId) {
  connection_ = createRecvConnection(service_, config_, channelId);

  if (!connection_) return false;

  if (config_.audio_recv_mode == AudioRecvAudioDataOnly
      && config_.audio_data_fetch_mode == AudioDataFetchPcmPull) {
    setupPuller();
  } else if (config_.audio_data_fetch_mode == AudioDataFetchPcmObserer) {
    auto local_user = connection_->GetLocalUser()->GetLocalUser();
    observer_ = setupAudioFrameObserver(config_.audio_data_observer_params, local_user);
  } else if (config_.audio_data_fetch_mode == AudioDataFetchMediaPacket) {
    setMediaPacketReceiver();
  }
  setupVideoReceiving();

  return true;
}

void MediaDataReceiver::waitForCompleted() {
  if (config_.audio_recv_mode == AudioRecvAudioDataOnly
      && config_.audio_data_fetch_mode == AudioDataFetchPcmPull) {
    if (pullerTread_ && pullerTread_->joinable()) {
      pullerTread_->join();
    }
  } else if (config_.audio_data_fetch_mode == AudioDataFetchPcmObserer) {
    connection_->GetLocalUser()->GetLocalUser()->unregisterAudioFrameObserver(observer_.get());
  } else if (config_.audio_data_fetch_mode == AudioDataFetchMediaPacket) {
    connection_->GetLocalUser()->GetLocalUser()->unregisterMediaControlPacketReceiver(
        media_packet_receiver_.get());

    auto remoteAudioTrack = connection_->GetLocalUser()->GetRemoteAudioTrack();
    if (remoteAudioTrack)
      remoteAudioTrack->unregisterMediaPacketReceiver(media_packet_receiver_.get());

    auto remoteVideoTrack = connection_->GetLocalUser()->GetRemoteVideoTrack();
    if (remoteVideoTrack)
      remoteVideoTrack->unregisterMediaPacketReceiver(media_packet_receiver_.get());
  }
}
