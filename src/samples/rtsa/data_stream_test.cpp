//
//  Agora RTSA SDK
//
//  Created by shenzewei in 2020-04.
//  Copyright (c) 2020 Agora IO. All rights reserved.
//

#include <string>
#include <memory>
#include <thread> // NOLINT

#include "gtest/gtest.h"

#include "wrapper/connection_wrapper.h"
#include "wrapper/utils.h"


const int ERR_NOT_SUPPORTED = -4;
const int ERR_TOO_OFTEN = -12;
const int ERR_SIZE_TOO_LARGE = -114;
const int ERR_BITRATE_LIMIT = -115;
const int ERR_TOO_MANY_DATA_STREAMS = -116;

const int DATA_STREAM_QPS = 60;
const int DATA_STREAM_BITRATE_BYTE = 30 * 1024;

DECLARE_SAMPLE_CASES(DataStreamApiTestRealVos)

#define RECEIVER_CREATE_AND_CONNECT() \
    ConnectionConfig recv_config; \
    recv_config.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER; \
    recv_config.recv_type = agora::rtc::RECV_PACKET_ONLY; \
    auto conn_recv = ConnectionWrapper::CreateConnection(service, recv_config); \
    ASSERT_TRUE(conn_recv->Connect(AGORA_APP_ID, CONNECTION_TEST_DEFAULT_CNAME, "222"))

#define SENDER_CREATE_AND_CONNECT() \
    ConnectionConfig send_config; \
    send_config.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER; \
    send_config.channelProfile = agora::CHANNEL_PROFILE_LIVE_BROADCASTING; \
    auto conn_send = ConnectionWrapper::CreateConnection(service, send_config); \
    ASSERT_TRUE(conn_send->Connect(AGORA_APP_ID, CONNECTION_TEST_DEFAULT_CNAME, "111"))

#define CHECK_RECV_DATA_STREAM_STATS(streamId) \
  std::this_thread::sleep_for(std::chrono::milliseconds(3000)); \
  EXPECT_TRUE(conn_recv->getDataStreamStats(conn_send->getConnectionInfo().localUserId.get()->c_str(), streamId, result)); \
  EXPECT_TRUE(result.check_result); \
  EXPECT_GT(result.received_msg_count, 0)


TEST_F(DataStreamApiTestRealVos, send_unordered_unreliable_message) {
  auto service = createAndInitAgoraService(false, true, true);

  RECEIVER_CREATE_AND_CONNECT();
  SENDER_CREATE_AND_CONNECT();

  // broadcast send message
  int send_stream_id = 0;
  EXPECT_EQ(0, conn_send->CreateDataStream(send_stream_id, false, false));

  std::string msg = std::string(1024, 0);
  int i = 0;
  for (auto iter = msg.begin(); iter != msg.end(); iter ++, i ++) *iter = ((i % 256) - 128);

  EXPECT_EQ(0, conn_send->SendStreamMessage(send_stream_id, msg.data(), msg.length()));
  DataStreamResult result;
  CHECK_RECV_DATA_STREAM_STATS(send_stream_id);

  conn_send->Disconnect();
  conn_recv->Disconnect();
}

TEST_F(DataStreamApiTestRealVos, testNotSupportedFeatures) {
  auto service = createAndInitAgoraService(false, true, true);

  RECEIVER_CREATE_AND_CONNECT();
  SENDER_CREATE_AND_CONNECT();

  int send_stream_id_1 = 0;
  int send_stream_id_2 = 0;
  EXPECT_EQ(ERR_NOT_SUPPORTED, conn_send->CreateDataStream(send_stream_id_1, true, false));
  EXPECT_EQ(ERR_NOT_SUPPORTED, conn_send->CreateDataStream(send_stream_id_2, false, true));

  conn_send->Disconnect();
  conn_recv->Disconnect();
}

TEST_F(DataStreamApiTestRealVos, createMaxDataStreams) {
  auto service = createAndInitAgoraService(false, true, true);

  RECEIVER_CREATE_AND_CONNECT();
  SENDER_CREATE_AND_CONNECT();

  int send_stream_id = 0;
  // test case : allow 5 data streams
  // stream 1/reliable false/ordered false
  conn_send->CreateDataStream(send_stream_id, false, false);

  // stream 2/reliable true/ordered true
  conn_send->CreateDataStream(send_stream_id, true, true);

  conn_send->CreateDataStream(send_stream_id, false, false);
  conn_send->CreateDataStream(send_stream_id, false, false);
  conn_send->CreateDataStream(send_stream_id, false, false);

  // stream 6: should fail afterward
  EXPECT_EQ(ERR_TOO_MANY_DATA_STREAMS, conn_send->CreateDataStream(send_stream_id, false, false));

  conn_send->Disconnect();
  conn_recv->Disconnect();
}

TEST_F(DataStreamApiTestRealVos, sendStreamMessage_Size_PPS) {
  auto service = createAndInitAgoraService(false, true, true);

  RECEIVER_CREATE_AND_CONNECT();
  SENDER_CREATE_AND_CONNECT();

  std::string msg;
  DataStreamResult result;

  int send_stream_id = 0;
  EXPECT_EQ(0, conn_send->CreateDataStream(send_stream_id, true, true));

  msg.resize(1025, 0);
  int i = 0;
  for (auto iter = msg.begin(); iter != msg.end(); iter ++, i ++) *iter = ((i % 256) - 128);
  EXPECT_EQ(ERR_SIZE_TOO_LARGE,
            conn_send->SendStreamMessage(send_stream_id, msg.data(), msg.length()));

  // max packet size: 1024 bytes
  msg.resize(1024, 0);
  i = 0;
  for (auto iter = msg.begin(); iter != msg.end(); iter ++, i ++) *iter = ((i % 256) - 128);
  conn_send->clearDataStreamStats(); // clear history stats.
  EXPECT_EQ(0, conn_send->SendStreamMessage(send_stream_id, msg.data(), msg.length()));
  CHECK_RECV_DATA_STREAM_STATS(send_stream_id); // check result.

  conn_send->clearDataStreamStats(); // clear history stats.
  msg.resize(10, 0);
  i = 0;
  for (auto iter = msg.begin(); iter != msg.end(); iter ++, i ++) *iter = ((i % 256) - 128);

  // call freq:60-packet/sec
  for (int i = 0; i < DATA_STREAM_QPS; i++) {
    EXPECT_EQ(0, conn_send->SendStreamMessage(send_stream_id, msg.data(), msg.length()));
  }
  // fail on 61st
  EXPECT_EQ(ERR_TOO_OFTEN,
            conn_send->SendStreamMessage(send_stream_id, msg.data(), msg.length()));
  CHECK_RECV_DATA_STREAM_STATS(send_stream_id); // check result.

  conn_send->clearDataStreamStats(); // clear history stats.
  // send after 1 second should be OK
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));
  EXPECT_EQ(0, conn_send->SendStreamMessage(send_stream_id, msg.data(), msg.length()));
  CHECK_RECV_DATA_STREAM_STATS(send_stream_id); // check result.

  conn_send->Disconnect();
  conn_recv->Disconnect();
}

TEST_F(DataStreamApiTestRealVos, sendStreamMessage_MaxRate6KB) {
  auto service = createAndInitAgoraService(false, true, true);

  RECEIVER_CREATE_AND_CONNECT();
  SENDER_CREATE_AND_CONNECT();

  int send_stream_id_1;
  int send_stream_id_2;
  EXPECT_EQ(0, conn_send->CreateDataStream(send_stream_id_1, true, true));
  EXPECT_EQ(0, conn_send->CreateDataStream(send_stream_id_2, true, true));

  std::string msg = std::string(1024, 0);
  int i = 0;
  for (auto iter = msg.begin(); iter != msg.end(); iter ++, i ++) *iter = ((i % 256) - 128);

  conn_send->clearDataStreamStats(); // clear history stats.
  
  int max_packets_per_stream = (DATA_STREAM_BITRATE_BYTE / msg.length() / 2); //  send DATA_STREAM_BITRATE_BYTE(30KB)
  for (int i = 0; i < max_packets_per_stream; ++i) {
    EXPECT_EQ(0, conn_send->SendStreamMessage(send_stream_id_1, msg.data(), msg.length()));
    EXPECT_EQ(0, conn_send->SendStreamMessage(send_stream_id_1, msg.data(), msg.length()));
  }

  EXPECT_EQ(ERR_BITRATE_LIMIT,
            conn_send->SendStreamMessage(send_stream_id_1, msg.data(), msg.length()));

  DataStreamResult result;
  CHECK_RECV_DATA_STREAM_STATS(send_stream_id_1); // check result.
  CHECK_RECV_DATA_STREAM_STATS(send_stream_id_1); // check result.

  conn_send->Disconnect();
  conn_recv->Disconnect();
}

