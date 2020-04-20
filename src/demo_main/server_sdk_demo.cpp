//  Agora RTC/MEDIA SDK
//
//  Created by Jay Zhang in 2020-04.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include <csignal>
#include <thread>
#include <string>
#include <sstream>

#include "wrapper/utils.h"
#include "wrapper/connection_wrapper.h"
#include "wrapper/local_user_wrapper.h"
#include "utils/opt_parser.h"
#include "utils/log.h"

#define DEFAULT_SAMPLE_RATE       (48000)
#define DEFAULT_NUM_OF_CHANNELS   (1)
#define DEFAULT_TARGET_BITRATE    (1*1000*1000)
#define DEFAULT_MIN_BITRATE       (500*1000)
#define DEFAULT_VIDEO_WIDTH       (640)
#define DEFAULT_VIDEO_HEIGHT      (360)
#define DEFAULT_FRAME_RATE        (10)
#define DEFAULT_AUDIO_FILE        "test_data/right_48khz.pcm"
#define DEFAULT_VIDEO_FILE        "test_data/video.yuv"

struct SampleOptions {
  std::string appId;
  std::string channelId;
  std::string userId;
  std::string audioFile = DEFAULT_AUDIO_FILE;
  std::string videoFile = DEFAULT_VIDEO_FILE;

  struct {
    int sampleRate = DEFAULT_SAMPLE_RATE;
    int numOfChannels = DEFAULT_NUM_OF_CHANNELS;
  } audio;
  struct {
    int targetBitrate = DEFAULT_TARGET_BITRATE;
    int width = DEFAULT_VIDEO_WIDTH;
    int height = DEFAULT_VIDEO_HEIGHT;
    int frameRate = DEFAULT_FRAME_RATE;
  } video;
};

static void SampleSendAudioFrame(const SampleOptions& options,
    agora::agora_refptr<agora::rtc::IAudioPcmDataSender> audioPcmDataSender) {
  static FILE *file = nullptr;
  const char* fileName = options.audioFile.c_str();

  // Calculate byte size for 10ms audio samples
  int sampleSize = sizeof(int16_t) * options.audio.numOfChannels;
  int samplesPer10ms = options.audio.sampleRate / 100;
  int sendBytes = sampleSize * samplesPer10ms;

  if (!file && !(file = fopen(fileName, "rb"))) {
    AG_LOG(ERROR, "open %s failed...\n", fileName);
    return;
  }

  uint8_t frameBuf[sendBytes];

  if (fread(frameBuf, 1, sizeof(frameBuf), file) != sizeof(frameBuf)) {
    if (ferror(file)) {
      AG_LOG(ERROR, "error reading audio file\n");
    } else if (feof(file)) {
      fclose(file);
      file = nullptr;
      AG_LOG(ERROR, "end of audio file\n");
    } else {
      AG_LOG(ERROR, "unknown error\n");
    }
    return;
  }

  if (audioPcmDataSender->sendAudioPcmData(frameBuf, 0, samplesPer10ms, sampleSize,
      options.audio.numOfChannels, options.audio.sampleRate) < 0) {
    AG_LOG(ERROR, "failed to send audio frame!\n");
  }
}

static void SampleSendVideoFrame(const SampleOptions& options,
    agora::agora_refptr<agora::rtc::IVideoFrameSender> videoFrameSender) {
  static FILE *file = nullptr;
  const char* fileName = options.videoFile.c_str();

  // Calculate byte size for YUV420 image
  int sendBytes = options.video.width * options.video.height * 3 / 2;

  if (!file && !(file = fopen(fileName, "rb"))) {
    AG_LOG(ERROR, "open %s failed...\n", fileName);
    return;
  }

  uint8_t frameBuf[sendBytes];

  if (fread(frameBuf, 1, sizeof(frameBuf), file) != sizeof(frameBuf)) {
    if (ferror(file)) {
      AG_LOG(ERROR, "error reading video file\n");
    } else if (feof(file)) {
      fclose(file);
      file = nullptr;
      AG_LOG(ERROR, "end of video file...\n");
    } else {
      AG_LOG(ERROR, "unknown error\n");
    }
    return;
  }

  agora::media::ExternalVideoFrame videoFrame;
  videoFrame.type = agora::media::ExternalVideoFrame::VIDEO_BUFFER_RAW_DATA;
  videoFrame.format = agora::media::VIDEO_PIXEL_I420;
  videoFrame.buffer = frameBuf;
  videoFrame.stride = options.video.width;
  videoFrame.height = options.video.height;
  videoFrame.cropLeft = 0;
  videoFrame.cropTop = 0;
  videoFrame.cropRight = 0;
  videoFrame.cropBottom = 0;
  videoFrame.rotation = 0;
  videoFrame.timestamp = 0;

  if (videoFrameSender->sendVideoFrame(videoFrame) < 0) {
    AG_LOG(ERROR, "failed to send video frame!\n");
  }
}

static bool stopFlag = false;
static void SignalHandler(int sigNo) {
  stopFlag = true;
}

int main(int argc, char* argv[]) {
  SampleOptions options;
  agora::base::opt_parser optParser;

  optParser.add_long_opt("appId", &options.appId, "App Id");
  optParser.add_long_opt("channelId", &options.channelId, "Channel Id");
  optParser.add_long_opt("userId", &options.userId, "User Id (default to be 0)");
  optParser.add_long_opt("audioFile", &options.audioFile, "Input audio file");
  optParser.add_long_opt("videoFile", &options.videoFile, "Input video file");
  optParser.add_long_opt("width", &options.video.width, "video width");
  optParser.add_long_opt("height", &options.video.height, "video height");
  optParser.add_long_opt("bitrate", &options.video.targetBitrate, "bitrate (bps)");

  if (!optParser.parse_opts(argc, argv)) {
    std::ostringstream strStream;
    optParser.print_usage(argv[0], strStream);
    std::cout << strStream.str() << std::endl;
    return -1;
  }

  if (options.appId.empty()) {
    AG_LOG(ERROR, "Must provide appId!\n");
    return -1;
  }

  if (options.channelId.empty()) {
    AG_LOG(ERROR, "Must provide channelId!\n");
    return -1;
  }

  signal(SIGQUIT, SignalHandler);
  signal(SIGABRT, SignalHandler);
  signal(SIGINT, SignalHandler);

  // Create Agora service
  auto service = createAndInitAgoraService(false, true, true);
  if (!service) {
    AG_LOG(ERROR, "failed to creating Agora service!\n");
  }

  // Create Agora connection
  ConnectionConfig config;
  config.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
  config.channelProfile = agora::CHANNEL_PROFILE_LIVE_BROADCASTING;
  config.sampleRateHz_ = options.audio.sampleRate;
  config.numberOfChannels_ = options.audio.numOfChannels;
  auto connection = ConnectionWrapper::CreateConnection(service, config);
  if (!connection) {
    AG_LOG(ERROR, "failed to creating Agora connection!\n");
    return -1;
  }

  // Connect to Agora channel
  bool connected = connection->Connect(options.appId.c_str(), options.channelId.c_str(),
      options.userId.c_str());
  if (!connected) {
    AG_LOG(ERROR, "failed to connect to Agora channel!\n");
    return -1;
  }

  // Create media node factory
  auto factory = service->createMediaNodeFactory();
  if (!factory) {
    AG_LOG(ERROR, "failed to create media node factory!\n");
  }

  // Create audio data sender
  auto audioPcmDataSender = factory->createAudioPcmDataSender();
  if (!audioPcmDataSender) {
    AG_LOG(ERROR, "failed to create audio data sender!\n");
    return -1;
  }

  // Create audio track
  agora::agora_refptr<agora::rtc::ILocalAudioTrack> customAudioTrack =
      service->createCustomAudioTrack(audioPcmDataSender);
  if (!customAudioTrack) {
    AG_LOG(ERROR, "failed to create audio track!\n");
    return -1;
  }

  // Publish audio track
  customAudioTrack->setEnabled(true);
  connection->GetLocalUser()->PublishAudioTrack(customAudioTrack);

  // Create video frame sender
  auto videoFrameSender = factory->createVideoFrameSender();
  if (!videoFrameSender) {
    AG_LOG(ERROR, "failed to create video frame sender!\n");
    return -1;
  }

  // Create video track
  agora::agora_refptr<agora::rtc::ILocalVideoTrack> customVideoTrack =
      service->createCustomVideoTrack(videoFrameSender);
  if (!customVideoTrack) {
    AG_LOG(ERROR, "failed to create video track!\n");
    return -1;
  }

  // Configure video encoder
  agora::rtc::VideoEncoderConfiguration encoderConfig(options.video.width,
                                              options.video.height,
                                              options.video.frameRate,
                                              options.video.targetBitrate,
                                              agora::rtc::ORIENTATION_MODE_ADAPTIVE);
  customVideoTrack->setVideoEncoderConfiguration(encoderConfig);

  // Publish video track
  customVideoTrack->setEnabled(true);
  connection->GetLocalUser()->PublishVideoTrack(customVideoTrack);

  // Start sending
  AG_LOG(INFO, "Start sending audio & video data ...\n");
  int i = 0;
  while (!stopFlag) {
    SampleSendAudioFrame(options, audioPcmDataSender);
    if (i++ % 10 == 0) {
      SampleSendVideoFrame(options, videoFrameSender);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Disconnect from Agora channel
  bool disconnected = connection->Disconnect();
  if (!disconnected) {
    AG_LOG(ERROR, "failed to disconnect from Agora channel!\n");
    return -1;
  }

  AG_LOG(INFO, "Disconnected from Agora channel successfully\n");
  return 0;
}
