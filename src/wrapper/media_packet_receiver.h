//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-08.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//
#pragma once
#include <sys/syscall.h>

#include "AgoraBase.h"
#include "IAgoraMediaEngine.h"
#include "api2/IAgoraService.h"
#include "api2/NGIAgoraRtcConnection.h"

#include "audio_frame_observer.h"

class MediaPacketReceiver : public agora::rtc::IMediaPacketReceiver,
                            public agora::rtc::IMediaControlPacketReceiver {
 public:
  MediaPacketReceiver();

  virtual ~MediaPacketReceiver();

  void SetVerbose(bool verbose);
  size_t GetReceivedMediaPacketBytes() { return received_media_packet_bytes_; }
  size_t GetReceivedControlPacketBytes() { return received_control_packet_bytes_; }

 public:
  // agora::rtc::IMediaPacketReceiver
  bool onMediaPacketReceived(const uint8_t* packet, size_t length) override;

 public:
  // agora::rtc::IMediaControlPacketReceiver
  bool onMediaControlPacketReceived(const uint8_t* packet, size_t length) override;

 private:
  bool verbose_{false};
  size_t received_media_packet_bytes_{0};
  size_t received_control_packet_bytes_{0};
};
