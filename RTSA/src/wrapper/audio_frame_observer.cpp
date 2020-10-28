//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-09.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#include <errno.h>
#include <cstring>
#include <sstream>

#include "audio_frame_observer.h"
#include "audio_pcm_frame_handler.h"
#include "audio_frame_handler_factory.h"

AudioFrameObserver::AudioFrameObserver(std::unique_ptr<AudioPCMFrameHandler> playbackDataHandler,
                                       std::unique_ptr<AudioPCMFrameHandler> recordingDataHandler,
                                       std::unique_ptr<AudioPCMFrameHandler> mixedDataHandler,
                                       std::unique_ptr<AudioFrameHandlerFactory> userDataHandlerFactory)
    : playbackDataHandler_(std::move(playbackDataHandler)),
      recordingDataHandler_(std::move(recordingDataHandler)),
      mixedDataHandler_(std::move(mixedDataHandler)),
      userDataHandlerFactory_(std::move(userDataHandlerFactory)),
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
  if (userDataHandlerFactory_) {
    for(auto item = pcm_data_handlers_.begin(); item != pcm_data_handlers_.end(); item ++)
      item->second->postHandleAudio();
  }
}

bool AudioFrameObserver::onPlaybackAudioFrame(AudioFrame& audioFrame) {
  if (!playbackDataHandler_.get()) return true;
  return handlePcmData(playbackDataHandler_.get(), audioFrame);
}

bool AudioFrameObserver::onRecordAudioFrame(AudioFrame& audioFrame) {
  if (!recordingDataHandler_.get()) return true;
  return handlePcmData(recordingDataHandler_.get(), audioFrame);
}

bool AudioFrameObserver::onMixedAudioFrame(AudioFrame& audioFrame) {
  if (!mixedDataHandler_.get()) return true;
  return handlePcmData(mixedDataHandler_.get(), audioFrame);
}

bool AudioFrameObserver::onPlaybackAudioFrameBeforeMixing(unsigned int uid,
                                                          AudioFrame& audioFrame) {
  if (!userDataHandlerFactory_.get()) return true;
  if (pcm_data_handlers_.find(uid) == pcm_data_handlers_.end()) {
    pcm_data_handlers_[uid] = std::move(userDataHandlerFactory_->createDefaultAudioHandler(uid));
    pcm_data_handlers_[uid]->preHandleAudio();
  }
  auto& handler = pcm_data_handlers_[uid];
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
