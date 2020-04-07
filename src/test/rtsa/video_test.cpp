//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include <thread>
#include <functional>

#include "gtest/gtest.h"

#include "wrapper/connection_wrapper.h"
#include "wrapper/local_user_wrapper.h"
#include "wrapper/media_packet_receiver.h"
#include "wrapper/media_packet_sender.h"
#include "wrapper/utils.h"
#include "wrapper/video_encoded_frame_receiver.h"
#include "wrapper/video_frame_sender.h"

class VideoTest : public testing::Test {
 public:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(VideoTest, h264_send_and_receive_encoded_frame) {
  auto service = createAndInitAgoraService(false, true, true);

  // Create receive connection
  ConnectionConfig recv_config;
  recv_config.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
  recv_config.subscribeAllAudio = true;

  recv_config.subscribeAllVideo = true;
  // encodedFrameOnly should be set to true when received encoded video frames
  recv_config.encodedFrameOnly = true;
  auto conn_recv = ConnectionWrapper::CreateConnection(service, recv_config);

  // Receive connection join channel
  ASSERT_TRUE(conn_recv->Connect(API_CALL_APPID, CONNECTION_TEST_DEFAULT_CNAME, "3"));

  // Create send connection and join channel
  ConnectionConfig config;
  config.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
  config.channelProfile = agora::CHANNEL_PROFILE_LIVE_BROADCASTING;

  auto conn_send = ConnectionWrapper::CreateConnection(service, config);
  ASSERT_TRUE(conn_send->Connect(API_CALL_APPID, CONNECTION_TEST_DEFAULT_CNAME, "1"));

  // Send connection start to send encoded video frames.
  auto factory = service->createMediaNodeFactory();

  std::unique_ptr<VideoH264FramesSender> video_frame_sender(new VideoH264FramesSender());
  video_frame_sender->initialize(service, factory, conn_send);
  auto thread = std::make_shared<std::thread>(
      std::bind(&VideoH264FramesSender::sendVideoFrames, video_frame_sender.get()));

  std::unique_ptr<VideoEncodedFrameReceiver> receiver(new VideoEncodedFrameReceiver);

  // Recv connection create and register receiver to received encoded video frame.
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  auto remote_video_track = conn_recv->GetLocalUser()->GetRemoteVideoTrack();
  ASSERT_TRUE(remote_video_track);
  remote_video_track->registerVideoEncodedImageReceiver(receiver.get());

  thread->join();

  // Unregister receiver after receiving.
  remote_video_track->unregisterVideoEncodedImageReceiver(receiver.get());

  auto received_video_frames = receiver->GetReceivedVideoFrames();
  AGO_LOG("Receive received video frames %d\n", received_video_frames);

  // Disconnect
  conn_send->Disconnect();
  conn_recv->Disconnect();
}

TEST_F(VideoTest, video_packet_send_and_receive) {
  auto service = createAndInitAgoraService(false, true, true);

  // Create receive connection
  ConnectionConfig recv_config;
  recv_config.clientRoleType = agora::rtc::CLIENT_ROLE_AUDIENCE;
  recv_config.subscribeAllAudio = true;
  recv_config.subscribeAllVideo = true;
  // encodedFrameOnly should be set to true when received encoded video frames
  recv_config.encodedFrameOnly = true;
  recv_config.recv_type = agora::rtc::RECV_PACKET_ONLY;

  auto conn_recv = ConnectionWrapper::CreateConnection(service, recv_config);

  // Receive connection join channel
  ASSERT_TRUE(conn_recv->Connect(API_CALL_APPID, CONNECTION_TEST_DEFAULT_CNAME, "3"));

  // Create send connection and join channel
  ConnectionConfig config;
  config.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
  config.channelProfile = agora::CHANNEL_PROFILE_LIVE_BROADCASTING;

  auto conn_send = ConnectionWrapper::CreateConnection(service, config);
  ASSERT_TRUE(conn_send->Connect(API_CALL_APPID, CONNECTION_TEST_DEFAULT_CNAME, "1"));

  // Send connection start to send encoded video frames.
  auto factory = service->createMediaNodeFactory();

  SendConfig args = {1500000, 1250, 7, false};
  std::unique_ptr<MediaPacketSender> packet_sender(new MediaPacketSender(args));
  packet_sender->initialize(service, factory, conn_send);

  auto thread = std::make_shared<std::thread>(
      std::bind(&MediaPacketSender::sendPackets, packet_sender.get()));

  std::unique_ptr<MediaPacketReceiver> receiver(new MediaPacketReceiver);

  // Recv connection create and register receiver to received encoded video frame.
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  auto remote_video_track = conn_recv->GetLocalUser()->GetRemoteVideoTrack();
  ASSERT_TRUE(remote_video_track);
  remote_video_track->registerMediaPacketReceiver(receiver.get());
  conn_recv->GetLocalUser()->GetLocalUser()->registerMediaControlPacketReceiver(receiver.get());

  thread->join();

  // Unregister receiver after receiving.
  conn_recv->GetLocalUser()->GetLocalUser()->unregisterMediaControlPacketReceiver(receiver.get());
  remote_video_track->unregisterMediaPacketReceiver(receiver.get());

  AGO_LOG("Receive media packet bytes %zu, control packet bytes %zu\n",
      receiver->GetReceivedMediaPacketBytes(),
      receiver->GetReceivedControlPacketBytes());

  // Disconnect
  conn_send->Disconnect();
  conn_recv->Disconnect();
}


