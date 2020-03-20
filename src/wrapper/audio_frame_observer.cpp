//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-09.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#include <errno.h>
#include <cstring>

#include "audio_frame_observer.h"

#include "audio_pcm_frame_handler.h"

AudioFrameObserver::AudioFrameObserver(std::unique_ptr<AudioPCMFrameHandler> playbackDataHandler,
                                       std::unique_ptr<AudioPCMFrameHandler> recordingDataHandler,
                                       std::unique_ptr<AudioPCMFrameHandler> mixedDataHandler,
                                       std::unique_ptr<AudioPCMFrameHandler> userDataHandler)
    : playbackDataHandler_(std::move(playbackDataHandler)),
      recordingDataHandler_(std::move(recordingDataHandler)),
      mixedDataHandler_(std::move(mixedDataHandler)),
      userDataHandler_(std::move(userDataHandler)),
      recvSamples(0) {
  if (playbackDataHandler_) {
    playbackDataHandler_->preHandleAudio();
  }
  if (recordingDataHandler_) {
    recordingDataHandler_->preHandleAudio();
  }
  if (mixedDataHandler_) {
    mixedDataHandler_->preHandleAudio();
  }
  if (userDataHandler_) {
    userDataHandler_->preHandleAudio();
  }
}

AudioFrameObserver::~AudioFrameObserver() {
  if (playbackDataHandler_) {
    playbackDataHandler_->postHandleAudio();
  }
  if (recordingDataHandler_) {
    recordingDataHandler_->postHandleAudio();
  }
  if (mixedDataHandler_) {
    mixedDataHandler_->postHandleAudio();
  }
  if (userDataHandler_) {
    userDataHandler_->postHandleAudio();
  }
}

bool AudioFrameObserver::onPlaybackAudioFrame(AudioFrame& audioFrame) {
  return handlePcmData(playbackDataHandler_.get(), audioFrame);
}

bool AudioFrameObserver::onRecordAudioFrame(AudioFrame& audioFrame) {
  return handlePcmData(recordingDataHandler_.get(), audioFrame);
}

bool AudioFrameObserver::onMixedAudioFrame(AudioFrame& audioFrame) {
  return handlePcmData(mixedDataHandler_.get(), audioFrame);
}

bool AudioFrameObserver::onPlaybackAudioFrameBeforeMixing(unsigned int uid,
                                                          AudioFrame& audioFrame) {
  if (pcm_data_handlers_.find(uid) == pcm_data_handlers_.end()) {
    char buff[128] = {0};
    snprintf(buff, sizeof(buff), "pcm_file_%u.wav", uid);
    pcm_data_handlers_[uid] = std::make_shared<AudioPCMFrameFileHandler>(buff, 2, 48000);
    pcm_data_handlers_[uid]->preHandleAudio();
  }
  //    printf("channels %d, samplesPerChannel %d, recvSamples %d\n", audioFrame.channels,
  //           audioFrame.samplesPerChannel, recvSamples);
  auto handler = pcm_data_handlers_[uid];
  void* payload_data = audioFrame.buffer;
  agora::rtc::AudioPcmDataInfo audioFrameInfo;
  audioFrameInfo.samplesOut = audioFrame.samplesPerChannel * audioFrame.channels;
  audioFrameInfo.elapsedTimeMs = audioFrame.renderTimeMs;
  handler->handlePcmData(payload_data, audioFrameInfo);
  recvSamples += audioFrameInfo.samplesOut;

  return true;
}

bool AudioFrameObserver::handlePcmData(AudioPCMFrameHandler* handler, AudioFrame& audioFrame) {
  if (handler) {
    void* payload_data = audioFrame.buffer;
    agora::rtc::AudioPcmDataInfo audioFrameInfo;
    audioFrameInfo.samplesOut = audioFrame.samplesPerChannel * audioFrame.channels;
    audioFrameInfo.elapsedTimeMs = audioFrame.renderTimeMs;
    handler->handlePcmData(payload_data, audioFrameInfo);
  }
  return true;
}
