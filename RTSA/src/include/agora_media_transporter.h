//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-08.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//
#pragma once
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

void parseArgs(int argc, char* argv[]);

void createAndInitializeAgoraService(bool enableAudioDevice = false,
                                     bool enableAudioProcessor = true, bool enableVideo = true);

void startConcurrentSend();

void startConcurrentPullRecv();

void startConcurrentObserverRecv();

void destroyAgoraService();

void getRecvType(bool& recv, int& type);

bool enableAudioDevice();
#ifdef __cplusplus
}
#endif
