//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//
#include "media_send_task.h"

#include "media_data_sender.h"
#include "wrapper/statistic_dump.h"
#include "wrapper/utils.h"

MediaSendTask::MediaSendTask(agora::base::IAgoraService* service, std::string threadName,
                             int cycles, bool sendAudio, bool sendVideo, bool sendMediaPacket,
                             int uid)
    : service_(service),
      threadName_(threadName),
      cycles_(cycles),
      sendAudio_(sendAudio),
      sendVideo_(sendVideo),
      mediaPacket_(sendMediaPacket),
      audioCodec_(agora::rtc::AUDIO_CODEC_OPUS),
      videoCodec_(agora::rtc::VIDEO_CODEC_H264),
      multiSlice_(false),
      uid_(uid) {}

MediaSendTask::~MediaSendTask() {}

void MediaSendTask::setAudioCodecType(agora::rtc::AUDIO_CODEC_TYPE audioCodec) {
  audioCodec_ = audioCodec;
}

void MediaSendTask::setVideoCodecType(agora::rtc::VIDEO_CODEC_TYPE videoCodec, bool multiSlice) {
  videoCodec_ = videoCodec;
  multiSlice_ = multiSlice;
}

void MediaSendTask::Run() {
  printf("To connect channel %s in thread %s, pid %d, tid %ld\n", threadName_.c_str(),
         threadName_.c_str(), getpid(), gettid());
  std::shared_ptr<MediaDataSender> audioVideoSender = std::make_shared<MediaDataSender>(service_, uid_);
  char buf[16] = {0};
  snprintf(buf, sizeof(buf), "%d", uid_);
  bool connected = audioVideoSender->connect(threadName_.c_str(), buf);
  if (connected) {
    printf("Connect successfully in channel name %s, uid %s to send stream cycles_ %d\n",
           threadName_.c_str(), buf, cycles_);
    for (int i = 0; i < cycles_; ++i) {
      if (sendAudio_) {
        printf("Start to send audio of round %d in thread %s\n", i, threadName_.c_str());
        if (mediaPacket_)
          audioVideoSender->sendAudioMediaPacket();
        else {
          switch (audioCodec_) {
            case agora::rtc::AUDIO_CODEC_AACLC:
              audioVideoSender->sendAudioAACFile("test_data/aac.aac", false);
              break;
            case agora::rtc::AUDIO_CODEC_HEAAC:
              audioVideoSender->sendAudioAACFile("test_data/he_aac.aac", true);
              break;
            case agora::rtc::AUDIO_CODEC_PCMU:
              audioVideoSender->sendAudioPcmFile("test_data/test.wav");
              break;
            case agora::rtc::AUDIO_CODEC_OPUS:
              audioVideoSender->sendAudioOpusFile(
                  "test_data/ehren-paper_lights-96.opus");
              break;
            default:
              break;
          }
        }
      }

      if (sendVideo_) {
        printf("Start to send video of round %d in thread %s\n", i, threadName_.c_str());
        if (mediaPacket_)
          audioVideoSender->sendVideoMediaPacket();
        else {
          switch (videoCodec_) {
            case agora::rtc::VIDEO_CODEC_VP8:
              audioVideoSender->sendVideoVp8File("test_data/test.vp8.ivf");
              break;
            case agora::rtc::VIDEO_CODEC_H264:
              if (multiSlice_) {
                audioVideoSender->sendVideoH264File(
                    "test_data/test_multi_slice.h264");
              } else {
                audioVideoSender->sendVideo();
              }
              break;
            default:
              break;
          }
        }
      }
      printf("Send audio/video of round %d end in thread %s\n", i, threadName_.c_str());
    }
  } else {
    printf("Connect to channel %s failed, tid %ld\n", threadName_.c_str(), gettid());
  }
  StatisticDump::dumpThreadFinalStats(gettid());
}
