//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//
#pragma once
#include <sys/syscall.h>
#include <unistd.h>
#include <thread>

#include "AgoraBase.h"
#include "api2/NGIAgoraRtcConnection.h"

#include "wrapper/audio_pcm_puller.h"
#include "wrapper/utils.h"

class AudioFrameObserver;
class ConnectionWrapper;
class MediaPacketReceiver;
class VideoEncodedFrameReceiver;

class MediaDataReceiver {
 public:
  MediaDataReceiver(agora::base::IAgoraService* service, const MediaDataRecvConfig &config);
  virtual ~MediaDataReceiver();

  void setVerbose(bool verbose);
  bool connect(const char* channelId);

  void waitForCompleted();

 private:
  void setupPuller();
  void setMediaPacketReceiver();
  void setupVideoReceiving();

 protected:
  agora::base::IAgoraService* service_;
  MediaDataRecvConfig config_;

  pid_t pid_;
  int64_t tid_;
  int statisticPacketNum_;

  std::shared_ptr<ConnectionWrapper> connection_;

  bool frameReceived_{false};
  bool verbose_{false};
  std::unique_ptr<VideoEncodedFrameReceiver> encodedFrameReceiver_;
  std::shared_ptr<AudioFrameObserver> observer_;
  std::shared_ptr<AudioPCMPuller> audioPuller_;
  std::shared_ptr<std::thread> pullerTread_;
  std::unique_ptr<MediaPacketReceiver> media_packet_receiver_;
};
