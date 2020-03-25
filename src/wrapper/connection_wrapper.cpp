//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include "connection_wrapper.h"

#include "local_user_wrapper.h"
#include "utils.h"

std::shared_ptr<ConnectionWrapper> ConnectionWrapper::CreateConnection(
    agora::base::IAgoraService* service, const ConnectionConfig& config) {
  agora::rtc::RtcConnectionConfiguration ccfg;
  // Set port range
  ccfg.minPort = config.minPort;
  ccfg.maxPort = config.maxPort;

  agora::rtc::AudioSubscriptionOptions audioSubOpt;
  if (!config.play) {
    audioSubOpt.pcmDataOnly = false;
    audioSubOpt.bytesPerSample = sizeof(int16_t) * config.numberOfChannels_;
    audioSubOpt.numberOfChannels = config.numberOfChannels_;
    audioSubOpt.sampleRateHz = config.sampleRateHz_;

    ccfg.audioSubscriptionOptions = audioSubOpt;
  }
  ccfg.clientRoleType = config.clientRoleType;
  ccfg.channelProfile = config.channelProfile;
  ccfg.recvType = config.recv_type;

  auto connection = service->createRtcConnection(ccfg);

  auto conn_wrapper = std::make_shared<ConnectionWrapper>(connection, config);
  return conn_wrapper;
}

ConnectionWrapper::ConnectionWrapper(agora::agora_refptr<agora::rtc::IRtcConnection> connection,
                                     const ConnectionConfig& config)
    : connection_(connection), connected_(false) {
  connection_->registerObserver(this);

  auto local_user = connection_->getLocalUser();
  local_user->setUserRole(config.clientRoleType);
  if (config.subscribeAllAudio) {
    local_user->subscribeAllAudio();
  }

  if (config.subscribeAllVideo) {
    agora::rtc::ILocalUser::VideoSubscriptionOptions subscriptionOptions;
    subscriptionOptions.type = config.type;
    subscriptionOptions.encodedFrameOnly = config.encodedFrameOnly;
    local_user->subscribeAllVideo(subscriptionOptions);
  }
  lcoal_user_wrapper_ = std::make_shared<LocalUserWrapper>(local_user);
}

ConnectionWrapper::~ConnectionWrapper() { connection_->unregisterObserver(this); }

bool ConnectionWrapper::Connect(const char* appid, const char* channelId, agora::user_id_t userId, int waitMs) {
  if (agora::ERR_OK == connection_->connect(appid, channelId, userId)) {
    connect_ready_.Wait(waitMs);
    if (connection_->getConnectionInfo().state == agora::rtc::CONNECTION_STATE_CONNECTED) {
      connected_ = true;
    } else {
      connected_ = false;
    }
  }
  return connected_;
}

bool ConnectionWrapper::Disconnect(int waitMs) {
  if (agora::ERR_OK == connection_->disconnect()){
    disconnect_ready_.Wait(waitMs);
    if (connection_->getConnectionInfo().state != agora::rtc::CONNECTION_STATE_CONNECTED) {
      connected_ = false;
    }
  }
  return !connected_;
}

std::shared_ptr<LocalUserWrapper> ConnectionWrapper::GetLocalUser() { return lcoal_user_wrapper_; }

void ConnectionWrapper::onConnected(const agora::rtc::TConnectionInfo& connectionInfo,
                                    agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) {
  connect_ready_.Set();
}

void ConnectionWrapper::onDisconnected(const agora::rtc::TConnectionInfo& connectionInfo,
                                       agora::rtc::CONNECTION_CHANGED_REASON_TYPE reason) {
  disconnect_ready_.Set();
}
