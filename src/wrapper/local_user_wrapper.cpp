//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include "local_user_wrapper.h"

LocalUserWrapper::LocalUserWrapper(agora::rtc::ILocalUser* local_user) : local_user_(local_user) {
  local_user_->registerLocalUserObserver(this);
}

LocalUserWrapper::~LocalUserWrapper() { local_user_->unregisterLocalUserObserver(this); }

agora::rtc::ILocalUser* LocalUserWrapper::GetLocalUser() { return local_user_; }

void LocalUserWrapper::PublishAudioTrack(
    agora::agora_refptr<agora::rtc::ILocalAudioTrack> audioTrack) {
  local_user_->publishAudio(audioTrack);
}

void LocalUserWrapper::PublishVideoTrack(
    agora::agora_refptr<agora::rtc::ILocalVideoTrack> videoTrack) {
  local_user_->publishVideo(videoTrack);
}

agora::agora_refptr<agora::rtc::IRemoteAudioTrack> LocalUserWrapper::GetRemoteAudioTrack(
    int wait_ms) {
  remote_audio_track_ready_.Wait(wait_ms);
  return remote_audio_track_;
}
agora::agora_refptr<agora::rtc::IRemoteVideoTrack> LocalUserWrapper::GetRemoteVideoTrack(
    int wait_ms) {
  remote_video_track_ready_.Wait(wait_ms);
  return remote_video_track_;
}

void LocalUserWrapper::onUserAudioTrackSubscribed(
    agora::user_id_t userId, agora::agora_refptr<agora::rtc::IRemoteAudioTrack> audioTrack) {
  remote_audio_track_ = audioTrack;
  remote_video_track_ready_.Set();
}

void LocalUserWrapper::onUserVideoTrackSubscribed(
    agora::user_id_t userId, agora::rtc::VideoTrackInfo trackInfo,
    agora::agora_refptr<agora::rtc::IRemoteVideoTrack> videoTrack) {
  remote_video_track_ = videoTrack;
  remote_video_track_ready_.Set();
}
