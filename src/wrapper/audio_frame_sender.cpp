//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include "audio_frame_sender.h"

#include <stdint.h>
#include <chrono>
#include <thread>

#include "connection_wrapper.h"
#include "local_user_wrapper.h"
#include "utils.h"
#include "utils/file_parser/audio_file_parser_factory.h"

AudioFrameSender::AudioFrameSender() = default;

AudioFrameSender::~AudioFrameSender() = default;

void AudioFrameSender::setVerbose(bool verbose) { verbose_ = verbose; }

EncodedAudioFrameSender::EncodedAudioFrameSender(const char* filepath, AUDIO_FILE_TYPE filetype)
    : file_path(filepath), file_type(filetype) {}

EncodedAudioFrameSender::~EncodedAudioFrameSender() = default;

bool EncodedAudioFrameSender::initialize(agora::base::IAgoraService* service,
                                         agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                                         std::shared_ptr<ConnectionWrapper> connection) {
  audio_encoded_frame_sender_ = std::move(factory->createAudioEncodedFrameSender());
  if (!audio_encoded_frame_sender_) {
    printf("Create audio encoded frame sender.\n");
    return false;
  }
  auto customAudioTrack =
      service->createCustomAudioTrack(audio_encoded_frame_sender_, agora::base::MIX_DISABLED);
  customAudioTrack->setEnabled(true);
  connection->GetLocalUser()->PublishAudioTrack(customAudioTrack);

  file_parser_ = std::move(
      AudioFileParserFactory::Instance().createAudioFileParser(file_path.c_str(), file_type));
  if (!file_parser_ || !file_parser_->open()) {
    printf("Open test file %s failed\n", file_path.c_str());
    return false;
  }
  printf("Open test file %s successfully\n", file_path.c_str());
  return true;
}

void EncodedAudioFrameSender::sendAudioFrames() {
  uint8_t databuf[8192] = {0};
  int length = 8192;
  int bytesnum = 0;
  agora::rtc::EncodedAudioFrameInfo audioFrameInfo;
  audioFrameInfo.numberOfChannels = file_parser_->getNumberOfChannels();
  audioFrameInfo.sampleRateHz = file_parser_->getSampleRateHz();
  audioFrameInfo.codec = file_parser_->getCodecType();

  AGO_LOG("sendAudio numberOfChannels %d, sampleRateHz %d, codec %d\n",
          audioFrameInfo.numberOfChannels, audioFrameInfo.sampleRateHz, audioFrameInfo.codec);

  int sentFrames = -1;
  auto startTime = std::chrono::steady_clock::now();
  while (file_parser_->hasNext()) {
    length = 8192;
    file_parser_->getNext(reinterpret_cast<char*>(databuf), &length);
    if (length > 0) {
      bool ret =
          audio_encoded_frame_sender_->sendEncodedAudioFrame(databuf, length, audioFrameInfo);
      if (!ret) {
        break;
      }
#if 0
      if (sentNumAudioFrames_ < 5) {
        dumpByteArray(buffer, length);
      }
#endif
      bytesnum += length;
      ++sent_audio_frames_;
      sentFrames++;
      auto sendFrameEndTime = std::chrono::steady_clock::now();
      int durationInMs = (sentFrames * 10) + 10;

      signed int diff = durationInMs - std::chrono::duration_cast<std::chrono::milliseconds>(
                                           sendFrameEndTime - startTime)
                                           .count();
      if (diff > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(diff));
      }
    }
  }
  if (verbose_) {
    AGO_LOG("Send %ld test aac frames end, %d bytes\n", sent_audio_frames_, bytesnum);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

AudioPcmFrameSender::AudioPcmFrameSender(const char* filepath) : file_path(filepath) {}

AudioPcmFrameSender::~AudioPcmFrameSender() = default;

bool AudioPcmFrameSender::initialize(agora::base::IAgoraService* service,
                                     agora::agora_refptr<agora::rtc::IMediaNodeFactory> factory,
                                     std::shared_ptr<ConnectionWrapper> connection) {
  audio_pcm_frame_ender_ = std::move(factory->createAudioPcmDataSender());
  if (!audio_pcm_frame_ender_) {
    return false;
  }
  auto customAudioTrack = service->createCustomAudioTrack(audio_pcm_frame_ender_);
  customAudioTrack->setEnabled(true);
  connection->GetLocalUser()->PublishAudioTrack(customAudioTrack);

  file_parser_ = AudioFileParserFactory::Instance().createAudioFileParser(
      file_path.c_str(), AUDIO_FILE_TYPE::AUDIO_FILE_PCM);
  ;
  if (!file_parser_ || !file_parser_->open()) {
    printf("Open test file %s failed\n", file_path.c_str());
    return false;
  }
  return true;
}

void AudioPcmFrameSender::sendAudioFrames() {
  const int loop_time_ms = -1;
  static constexpr int BufferSize = 4096;
  int samples_per_loop = file_parser_->getSampleRateHz() / 100;
  int sample_size = file_parser_->getNumberOfChannels() * file_parser_->getBitsPerSample() / 8;

  unsigned char dataBuffer[BufferSize] = {0};
  int length = 0;
  int frames = 0;

  auto start_time = now_ms();
  while (file_parser_->hasNext()) {
    file_parser_->getNext(reinterpret_cast<char*>(dataBuffer), &length);
    auto overhead_begin = now_ms();
    if ((loop_time_ms != -1) && (overhead_begin - start_time) >= loop_time_ms) break;
    audio_pcm_frame_ender_->sendAudioPcmData(dataBuffer, 0, samples_per_loop, sample_size,
                                             file_parser_->getNumberOfChannels(),
                                             file_parser_->getSampleRateHz());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}
