//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#pragma once

#include "AgoraBase.h"
#include "api2/NGIAgoraLocalUser.h"

#include "utils/auto_reset_event.h"

class LocalUserWrapper : public agora::rtc::ILocalUserObserver {
 public:
  LocalUserWrapper(agora::rtc::ILocalUser* local_user);
  virtual ~LocalUserWrapper();

 public:
  agora::rtc::ILocalUser* GetLocalUser();
  void PublishAudioTrack(agora::agora_refptr<agora::rtc::ILocalAudioTrack> audioTrack);
  void PublishVideoTrack(agora::agora_refptr<agora::rtc::ILocalVideoTrack> audioTrack);
  agora::agora_refptr<agora::rtc::IRemoteAudioTrack> GetRemoteAudioTrack() { return remote_audio_track_; }
  agora::agora_refptr<agora::rtc::IRemoteVideoTrack> GetRemoteVideoTrack() { return remote_video_track_; }

  void setMediaPacketReceiver(agora::rtc::IMediaPacketReceiver* receiver) {
    std::lock_guard<std::mutex> _(observer_lock_);
    media_packet_receiver_ = receiver;
    if (remote_audio_track_)
      remote_audio_track_->registerMediaPacketReceiver(media_packet_receiver_);

    if (remote_video_track_)
      remote_video_track_->registerMediaPacketReceiver(media_packet_receiver_);
  }

  void setVideoEncodedImageReceiver(agora::rtc::IVideoEncodedImageReceiver* receiver) {
    video_encoded_receiver_ = receiver;
  }

 public:
  // inherit from agora::rtc::ILocalUserObserver
  void onAudioTrackPublishSuccess(
      agora::agora_refptr<agora::rtc::ILocalAudioTrack> audioTrack) override {}

  void onUserAudioTrackSubscribed(
      agora::user_id_t userId,
      agora::agora_refptr<agora::rtc::IRemoteAudioTrack> audioTrack) override;

  void onAudioTrackPublicationFailure(agora::agora_refptr<agora::rtc::ILocalAudioTrack> audioTrack,
                                      agora::ERROR_CODE_TYPE error) override {}

  void onUserAudioTrackStateChanged(agora::user_id_t userId,
                                    agora::agora_refptr<agora::rtc::IRemoteAudioTrack> audioTrack,
                                    agora::rtc::REMOTE_AUDIO_STATE state,
                                    agora::rtc::REMOTE_AUDIO_STATE_REASON reason,
                                    int elapsed) override {}

  void onLocalAudioTrackStateChanged(agora::agora_refptr<agora::rtc::ILocalAudioTrack> audioTrack,
                                     agora::rtc::LOCAL_AUDIO_STREAM_STATE state,
                                     agora::rtc::LOCAL_AUDIO_STREAM_ERROR errorCode) override {}

  void onVideoTrackPublishSuccess(
      agora::agora_refptr<agora::rtc::ILocalVideoTrack> videoTrack, int elapsed) override {}

  void onVideoTrackPublicationFailure(agora::agora_refptr<agora::rtc::ILocalVideoTrack> videoTrack,
                                      agora::ERROR_CODE_TYPE error) override {}

  void onUserVideoTrackSubscribed(
      agora::user_id_t userId, agora::rtc::VideoTrackInfo trackInfo,
      agora::agora_refptr<agora::rtc::IRemoteVideoTrack> videoTrack) override;

  void onUserVideoTrackStateChanged(agora::user_id_t userId,
                                    agora::agora_refptr<agora::rtc::IRemoteVideoTrack> videoTrack,
                                    agora::rtc::REMOTE_VIDEO_STREAM_STATE state,
                                    agora::rtc::REMOTE_VIDEO_STATE_REASON reason, int elapsed) override {}

  void onRemoteVideoTrackStatistics(agora::agora_refptr<agora::rtc::IRemoteVideoTrack> videoTrack,
                                    const agora::rtc::RemoteVideoTrackStats& stats) override {}

  void onLocalVideoTrackStateChanged(agora::agora_refptr<agora::rtc::ILocalVideoTrack> videoTrack,
                                     agora::rtc::LOCAL_VIDEO_STREAM_STATE state,
                                     agora::rtc::LOCAL_VIDEO_STREAM_ERROR errorCode) override {}

  void onLocalVideoTrackStatistics(agora::agora_refptr<agora::rtc::ILocalVideoTrack> videoTrack,
                                   const agora::rtc::LocalVideoTrackStats& stats) override {}

  void onAudioVolumeIndication(const agora::rtc::AudioVolumeInfo* speakers,
                               unsigned int speakerNumber, int totalVolume) override {}

  void onLocalAudioTrackStatistics(const agora::rtc::LocalAudioStats& stats) override {}

  void onRemoteAudioTrackStatistics(agora::agora_refptr<agora::rtc::IRemoteAudioTrack> audioTrack,
                                    const agora::rtc::RemoteAudioTrackStats& stats) override {}

 private:
  agora::rtc::ILocalUser* local_user_{nullptr};

  agora::agora_refptr<agora::rtc::IRemoteAudioTrack> remote_audio_track_;
  agora::agora_refptr<agora::rtc::IRemoteVideoTrack> remote_video_track_;

  agora::rtc::IMediaPacketReceiver* media_packet_receiver_{nullptr};
  agora::rtc::IVideoEncodedImageReceiver* video_encoded_receiver_{nullptr};

  std::mutex observer_lock_;
};
