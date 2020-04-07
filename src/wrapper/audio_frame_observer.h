//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-09.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once
#include <stdio.h>
#include <map>
#include <memory>
#include <string>

#include "IAgoraMediaEngine.h"

class AudioPCMFrameHandler;
class AudioFrameHandlerFactory;

class AudioFrameObserver : public agora::media::IAudioFrameObserver {
 public:
  AudioFrameObserver(std::unique_ptr<AudioPCMFrameHandler> playbackDataHandler,
                     std::unique_ptr<AudioPCMFrameHandler> recordingDataHandler,
                     std::unique_ptr<AudioPCMFrameHandler> mixedDataHandler,
                     std::unique_ptr<AudioFrameHandlerFactory> userDataHandlerFactory);

  virtual ~AudioFrameObserver();

  bool onPlaybackAudioFrame(AudioFrame& audioFrame) override;

  bool onRecordAudioFrame(AudioFrame& audioFrame) override;

  bool onMixedAudioFrame(AudioFrame& audioFrame) override;

  bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame) override;

 private:
  bool handlePcmData(AudioPCMFrameHandler* handler, AudioFrame& audioFrame);

 private:
  std::unique_ptr<AudioPCMFrameHandler> playbackDataHandler_;
  std::unique_ptr<AudioPCMFrameHandler> recordingDataHandler_;
  std::unique_ptr<AudioPCMFrameHandler> mixedDataHandler_;
  std::unique_ptr<AudioFrameHandlerFactory> userDataHandlerFactory_;
  std::map<unsigned int, std::unique_ptr<AudioPCMFrameHandler>> pcm_data_handlers_;
  int recvSamples;
};
