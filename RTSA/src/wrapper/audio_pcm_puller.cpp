//
//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-08.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#include "audio_pcm_puller.h"

#include <stdlib.h>
#include <string.h>
#include <thread>

#include "audio_pcm_frame_handler.h"
#include "utils.h"

AudioPCMPuller::AudioPCMPuller(const agora::rtc::AudioPcmDataInfo& audioFrameInfo,
                               size_t numberOfChannels, size_t bytesPerSample,
                               uint32_t sampleRateHz, uint64_t duration,
                               agora::rtc::ILocalUser* audioFramePuller)
    : audioFrameInfo_(audioFrameInfo),
      numberOfChannels_(numberOfChannels),
      bytesPerSample_(bytesPerSample),
      sampleRateHz_(sampleRateHz),
      duration_(duration),
      audioFramePuller_(audioFramePuller),
      endTime_(0) {}

AudioPCMPuller::~AudioPCMPuller() = default;

void AudioPCMPuller::setAudioPcmFrameHandler(std::unique_ptr<AudioPCMFrameHandler> handler) {
  auido_frame_handler_ = std::move(handler);
}

void AudioPCMPuller::startPullAudioPCM() {
  auido_frame_handler_->preHandleAudio();

  int16_t* audiobuffer = new int16_t[sampleRateHz_ / 100 * numberOfChannels_ * bytesPerSample_];
  volatile uint64_t now = now_ms();
  endTime_ = now + duration_;
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  while (now < endTime_) {
    uint64_t frameStartTime = now_ms();
    audioFrameInfo_.samplesOut = 0;
    audioFrameInfo_.elapsedTimeMs = 0;
    audioFrameInfo_.ntpTimeMs = 0;
    audioFramePuller_->pullMixedAudioPcmData(audiobuffer, audioFrameInfo_);

    if (!auido_frame_handler_->handlePcmData(audiobuffer, audioFrameInfo_)) {
      break;
    }

    uint64_t frameEndTime = now_ms();
    std::this_thread::sleep_for(std::chrono::milliseconds(10 - (frameEndTime - frameStartTime)));
    now = now_ms();
  }
  delete[] audiobuffer;
  uint64_t endTime = now_ms();

  auido_frame_handler_->postHandleAudio();
}
