//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "utils.h"
#include "wrapper/audio_frame_observer.h"
#include "wrapper/audio_pcm_frame_handler.h"
#include "wrapper/audio_pcm_puller.h"
#include "wrapper/connection_wrapper.h"

void dumpByteArray(const char* byteArray, int size) {
  int i = 0;
  fprintf(stdout, "\n0x");
  while (i < size) {
    fprintf(stdout, "%02x ", (0xFF & byteArray[i]));
    ++i;
    if (i % 16 == 0) {
      fprintf(stdout, " \n0x");
    } else if (i % 4 == 0) {
      fprintf(stdout, " ");
    }
  }
  fprintf(stdout, "\n");
  fprintf(stdout, " \n");
}

std::string generateChannelName(int postfix, const char* cname, bool containPidInfo) {
  static const pid_t pid = getpid();
  char threadname[256] = {0};

  if (postfix == 0) {
    if (containPidInfo) {
      snprintf(threadname, sizeof(threadname), "%s_%05d", cname, pid);
    } else {
      snprintf(threadname, sizeof(threadname), "%s", cname);
    }
  } else {
    if (containPidInfo) {
      snprintf(threadname, sizeof(threadname), "%s_%05d_%04d", cname, pid, postfix);
    } else {
      snprintf(threadname, sizeof(threadname), "%s_%04d", cname, postfix);
    }
  }
  std::string threadName(threadname);
  return threadName;
}

agora::base::IAgoraService* createAndInitAgoraService(bool enableAudioDevice,
                                                      bool enableAudioProcessor, bool enableVideo) {
  auto service = createAgoraService();
  agora::base::AgoraServiceConfiguration scfg;
  scfg.enableAudioProcessor = enableAudioProcessor;
  scfg.enableAudioDevice = enableAudioDevice;
  scfg.enableVideo = enableVideo;

  service->initialize(scfg);

  return service;
}

std::shared_ptr<ConnectionWrapper> createRecvConnection(agora::base::IAgoraService* service,
    const char* channelId, MediaDataRecvConfig &recv_config) {
  ConnectionConfig config;
  config.minPort = recv_config.minPort;
  config.maxPort = recv_config.maxPort;

  if (recv_config.audio_recv_mode == AudioRecvAudioDataOnly
      && recv_config.audio_data_fetch_mode == AudioDataFetchPcmPull) {
    config.numberOfChannels_ = recv_config.audio_data_pull_param.numberOfChannels;
    config.sampleRateHz_ = recv_config.audio_data_pull_param.sampleRateHz;
  } else if (recv_config.audio_data_fetch_mode == AudioDataFetchMediaPacket) {
    config.recv_type = agora::rtc::RECV_PACKET_ONLY;
  }

  config.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
  config.subscribeAllAudio = true;
  config.subscribeAllVideo = true;

  if (recv_config.video_recv_mode == VideoRecvEncodedFrame) {
    config.encodedFrameOnly = true;
  }

  auto connection_ = ConnectionWrapper::CreateConnection(service, config);

  char buf[16] = {0};
  snprintf(buf, sizeof(buf), "%d", recv_config.uid);

  connection_->Connect(API_CALL_APPID, channelId, buf);

  if (!connection_->WaitForConnected(3000)) {
    printf("Connect to channel %s failed, tid %ld\n", channelId, gettid());
    return nullptr;
  }
  printf("Connect to channel %s uid %s successfully!\n", channelId, buf);

  return connection_;
}

std::string genPullFileName(int32_t uid) {
  char pullerd_audio_pcm_filename[64] = { 0 };
  if (uid == 0) {
    snprintf(pullerd_audio_pcm_filename, sizeof(pullerd_audio_pcm_filename), "%s.wav", "pulled_audio_pcm");
  } else {
    snprintf(pullerd_audio_pcm_filename, sizeof(pullerd_audio_pcm_filename), "%s_%03d.wav",
        "pulled_audio_pcm", uid);
  }
  return pullerd_audio_pcm_filename;
}

std::shared_ptr<AudioPCMPuller> createAudioPcmPuller(agora::rtc::ILocalUser* audioPuller,
    AudioDataFetchParams &params, uint64_t duration) {
  agora::rtc::AudioPcmDataInfo audioFrameInfo;
  audioFrameInfo.sampleCount = params.sampleRateHz / 100;  // 10 ms data
  size_t bytesPerSample = sizeof(int16_t) * params.numberOfChannels;
  auto puller = std::make_shared<AudioPCMPuller>(audioFrameInfo, params.numberOfChannels, bytesPerSample,
      params.sampleRateHz, duration, audioPuller);
  if (params.save_file) {
    std::unique_ptr<AudioPCMFrameFileHandler> handler(
        new AudioPCMFrameFileHandler(params.file_saved_path, params.numberOfChannels, params.sampleRateHz));
    puller->setAudioPcmFrameHandler(std::move(handler));
  } else {
    std::unique_ptr<AudioPCMFrameCheckerHandler> handler(new AudioPCMFrameCheckerHandler);
    puller->setAudioPcmFrameHandler(std::move(handler));
  }
  return puller;
}

std::unique_ptr<AudioPCMFrameHandler> createAudioFrameHandler(AudioDataFetchParams params) {
  size_t numberOfChannels = params.numberOfChannels;
  uint32_t sampleRateHz = params.sampleRateHz;
  bool save_file = params.save_file;

  std::unique_ptr<AudioPCMFrameHandler> handler;
  if (save_file) {
    handler.reset(new AudioPCMFrameFileHandler(params.file_saved_path, numberOfChannels, sampleRateHz));
  } else {
    handler.reset(new AudioPCMFrameCheckerHandler);
  }
  return std::move(handler);
}


std::shared_ptr<AudioFrameObserver> setupAudioFrameObserver(const AudioDataObserverParams &params, agora::rtc::ILocalUser* local_user) {
  if (params.audio_data_fetch_type == AudioDataTypeNone) {
    return nullptr;
  }

  std::unique_ptr<AudioPCMFrameHandler> recording_data_handler;
  std::unique_ptr<AudioPCMFrameHandler> playback_data_handler;
  std::unique_ptr<AudioPCMFrameHandler> user_data_handler;
  std::unique_ptr<AudioPCMFrameHandler> mixed_data_handler;

  if (params.audio_data_observer_params[0].numberOfChannels != 0 && params.audio_data_observer_params[0].sampleRateHz != 0) {
    int ret = local_user->setRecordingAudioFrameParameters(params.audio_data_observer_params[0].numberOfChannels,
        params.audio_data_observer_params[0].sampleRateHz);
    printf("setRecordingAudioFrameParameters ret %d, tid %ld\n", ret, gettid());

    recording_data_handler = std::move(createAudioFrameHandler(params.audio_data_observer_params[0]));
  }

  if (params.audio_data_observer_params[1].numberOfChannels != 0 && params.audio_data_observer_params[1].sampleRateHz != 0) {
    int ret = local_user->setPlaybackAudioFrameParameters(params.audio_data_observer_params[1].numberOfChannels,
        params.audio_data_observer_params[1].sampleRateHz);
    printf("setPlaybackAudioFrameParameters ret %d, tid %ld\n", ret, gettid());

    playback_data_handler = std::move(createAudioFrameHandler(params.audio_data_observer_params[1]));
  }

  if (params.audio_data_observer_params[2].numberOfChannels != 0 && params.audio_data_observer_params[2].sampleRateHz != 0) {
    int ret = local_user->setPlaybackAudioFrameBeforeMixingParameters(params.audio_data_observer_params[2].numberOfChannels,
        params.audio_data_observer_params[2].sampleRateHz);
    printf("setPlaybackAudioFrameBeforeMixingParameters ret %d, tid %ld\n", ret, gettid());
    user_data_handler = std::move(createAudioFrameHandler(params.audio_data_observer_params[2]));
  }

  if (params.audio_data_observer_params[3].numberOfChannels != 0 && params.audio_data_observer_params[3].sampleRateHz != 0) {
    int ret = local_user->setMixedAudioFrameParameters(params.audio_data_observer_params[3].numberOfChannels,
        params.audio_data_observer_params[3].sampleRateHz);
    printf("setMixedAudioFrameParameters ret %d, tid %ld\n", ret, gettid());
    mixed_data_handler = std::move(createAudioFrameHandler(params.audio_data_observer_params[3]));
  }

  auto observer = std::make_shared<AudioFrameObserver>(
      std::move(playback_data_handler), std::move(recording_data_handler),
      std::move(mixed_data_handler), std::move(user_data_handler));

  local_user->registerAudioFrameObserver(observer.get());

  return observer;
}

