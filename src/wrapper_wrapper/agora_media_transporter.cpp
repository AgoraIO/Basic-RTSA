//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#include "agora_media_transporter.h"

#include <functional>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <vector>

#include "api2/IAgoraService.h"

#include "media_data_receiver.h"
#include "media_data_sender.h"
#include "media_send_task.h"
#include "wrapper/utils.h"

static agora::base::IAgoraService* sService = nullptr;

static int concurrency = 1;
static int cycles = 1;
static int audioCodec = 1;
static int videoCodec = 2;
static int startUid = 0;
static uint64_t duration = 200000;
static int sendMedia = 2;
static bool multiSlice = false;
static int fileSaveOpt = -1;
static std::string fileSaveNamePath = "user_pcm_audio_data.wav";
static bool isRecv = false;
static int recvType = 0;
static bool mediaPacket = false;
static std::string connection_test_cname = CONNECTION_TEST_DEFAULT_CNAME;
static bool startRecorder = false;

void parseArgs(int argc, char* argv[]) {
  char* ptr = nullptr;
  int ch = 0;
  while ((ch = getopt(argc, argv, "a:v:j:d:hm:n:u:s:r:pc:l")) != -1) {
    switch (ch) {
      case 'a':
        audioCodec = atoi(optarg);
        break;
      case 'v':
        videoCodec = atoi(optarg);
        break;
      case 'j':
        concurrency = atoi(optarg);
        break;
      case 'd':
        duration = strtoul(optarg, &ptr, 10);
        break;
      case 'h':
        multiSlice = true;
        break;
      case 'm':
        sendMedia = atoi(optarg);
        break;
      case 'n':
        cycles = atoi(optarg);
        break;
      case 'u':
        startUid = atoi(optarg);
        break;
      case 's': {
        int opt = atoi(optarg);
        if (opt < 0 || opt > 3)
          printf("Illegal save option, the accpeted ones are 0/1/2/3 ...\n");
        else
          fileSaveOpt = opt;
      } break;
      case 'r':
        isRecv = true;
        recvType = atoi(optarg);
        break;
      case 'p':
        mediaPacket = true;
        break;
      case 'c':
        connection_test_cname = optarg;
        break;
      case 'l':
        startRecorder = true;
        break;
      case '?':
        printf("Unknown option: %c\n", static_cast<char>(optopt));
        break;
    }
  }
  printf("Concurrency %d, cycles %d\n", concurrency, cycles);
}

void createAndInitializeAgoraService(bool enableAudioDevice, bool enableAudioProcessor,
                                     bool enableVideo) {
  sService = createAndInitAgoraService(enableAudioDevice, enableAudioProcessor, enableVideo);
  if (!sService) {
    printf("Fail to create or init service ...\n");
    std::exit(1);
  }
}

agora::rtc::AUDIO_CODEC_TYPE getAudioCodecType(int audioCodec) {
  agora::rtc::AUDIO_CODEC_TYPE audioCodecType = agora::rtc::AUDIO_CODEC_OPUS;
  switch (audioCodec) {
    case 1:
      audioCodecType = agora::rtc::AUDIO_CODEC_OPUS;
      break;
    case 3:
      audioCodecType = agora::rtc::AUDIO_CODEC_PCMU;
      break;
    case 7:
      audioCodecType = agora::rtc::AUDIO_CODEC_AACLC;
      break;
    case 8:
      audioCodecType = agora::rtc::AUDIO_CODEC_HEAAC;
      break;
  }
  return audioCodecType;
}

void getRecvType(bool& recv, int& type) {
  recv = isRecv;
  type = recvType;
}

bool enableAudioDevice() { return startRecorder && isRecv && (recvType == 1); }

enum VIDEO_CODEC_TYPE {
  /** 1: VP8. */
  VIDEO_CODEC_VP8 = 1,
  /** 2: h264. */
  VIDEO_CODEC_H264 = 2,
  /** 5: VP9. */
  VIDEO_CODEC_VP9 = 5,
  // kVideoCodecI420,
  // kVideoCodecRED,
  // kVideoCodecULPFEC,
  // kVideoCodecFlexfec,
  // kVideoCodecGeneric,
  // kVideoCodecMultiplex,
  // kVideoCodecUnknown
};

agora::rtc::VIDEO_CODEC_TYPE getVideoCodecType(int videoCodec) {
  agora::rtc::VIDEO_CODEC_TYPE videoCodecType = agora::rtc::VIDEO_CODEC_H264;
  switch (videoCodec) {
    case 1:
      videoCodecType = agora::rtc::VIDEO_CODEC_VP8;
      break;
    case 2:
      videoCodecType = agora::rtc::VIDEO_CODEC_H264;
      break;
  }
  return videoCodecType;
}

void startConcurrentSend() {
  printf("start concurrent send...\n");
  bool sendVideo = ((sendMedia & 1) != 0);
  bool sendAudio = ((sendMedia & 2) != 0);

  std::vector<std::shared_ptr<MediaSendTask>> tasks;
  std::vector<std::thread*> sysThreads;

  for (int i = 0; i < concurrency; ++i) {
    std::shared_ptr<MediaSendTask> task = std::make_shared<MediaSendTask>(
        sService, generateChannelName(i + startUid, connection_test_cname.c_str(), false), cycles,
        sendAudio, sendVideo, mediaPacket, 2 * (i + startUid) + 3);
    task->setAudioCodecType(getAudioCodecType(audioCodec));
    task->setVideoCodecType(getVideoCodecType(videoCodec), multiSlice);
    tasks.push_back(task);
    std::thread* systhread = new std::thread(std::bind(&MediaSendTask::Run, task.get()));
    sysThreads.push_back(systhread);

    std::this_thread::sleep_for(std::chrono::milliseconds(739));
  }

  for (auto threadIt = sysThreads.begin(); threadIt != sysThreads.end(); ++threadIt) {
    (*threadIt)->join();
  }
  while (!sysThreads.empty()) {
    std::thread* systhread = sysThreads[sysThreads.size() - 1];
    sysThreads.pop_back();
    delete systhread;
  }
}

void startConcurrentPullRecv() {
  printf("start concurrent pull recv...\n");
  std::vector<std::shared_ptr<MediaDataReceiver>> receivers;
  for (int i = 0; i < concurrency; ++i) {
    MediaDataRecvConfig config;
    config.duration = duration;
    config.uid = 2 * (i + startUid) + 6;
    config.audio_data_fetch_mode = AudioDataFetchPcmPull;
    config.audio_data_pull_param.save_file = true;
    config.audio_data_pull_param.file_saved_path = genPullFileName(config.uid);

    auto mediaReceiver = std::make_shared<MediaDataReceiver>(sService, config);
    auto channelName = generateChannelName(i + startUid, connection_test_cname.c_str(), false);

    mediaReceiver->connect(channelName.c_str());
    receivers.emplace_back(mediaReceiver);

    std::this_thread::sleep_for(std::chrono::milliseconds(739));
  }

  printf("receivers count %zu\n", receivers.size());
  std::this_thread::sleep_for(std::chrono::milliseconds(duration));
  for (auto receiver = receivers.begin(); receiver != receivers.end(); ++receiver) {
    (*receiver)->waitForCompleted();
  }
}

void startConcurrentObserverRecv() {
  printf("start concurrent observer recv...\n");
  std::vector<std::shared_ptr<MediaDataReceiver>> receivers;
  for (int i = 0; i < concurrency; ++i) {
    printf("start i %d \n", i);
    MediaDataRecvConfig config;
    config.duration = duration;
    config.uid = 2 * (i + startUid) + 6;

    if (mediaPacket) {
      config.audio_data_fetch_mode = AudioDataFetchMediaPacket;
    } else {
      config.audio_data_fetch_mode = AudioDataFetchPcmObserer;
      config.audio_data_observer_params.audio_data_fetch_type = static_cast<AudioDataFetchType>(AudioDataTypePlayback
          | AudioDataTypePlaybackBeforeMixing);
      if (fileSaveOpt != -1) {
        config.audio_data_observer_params.audio_data_observer_params[fileSaveOpt].save_file = true;
        config.audio_data_observer_params.audio_data_observer_params[fileSaveOpt].file_saved_path = fileSaveNamePath;
      }
    }

    auto mediaReceiver = std::make_shared<MediaDataReceiver>(sService, config);
    auto channelName = generateChannelName(i + startUid, connection_test_cname.c_str(), false);
    mediaReceiver->connect(channelName.c_str());

    receivers.emplace_back(mediaReceiver);

    std::this_thread::sleep_for(std::chrono::milliseconds(739));
  }

  printf("receivers count %zu\n", receivers.size());
  std::this_thread::sleep_for(std::chrono::milliseconds(duration));
  for (auto receiver = receivers.begin(); receiver != receivers.end(); ++receiver) {
    (*receiver)->waitForCompleted();
  }
}

void destroyAgoraService() {
  sService->release();
  sService = nullptr;
}
