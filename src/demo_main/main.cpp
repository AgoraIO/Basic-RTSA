//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory>
#include <thread>

#include "agora_media_transporter.h"

int main(int argc, char* argv[]) {
  std::unique_ptr<char* []> tmpagrv(new char*[argc]);
  for (int i = 0; i < argc; ++i) {
    tmpagrv[i] = argv[i];
  }

  parseArgs(argc, argv);

  bool recv = false;
  int type = 0;
  getRecvType(recv, type);

  createAndInitializeAgoraService(enableAudioDevice());

  std::this_thread::sleep_for(std::chrono::milliseconds(739));

  if (recv) {
    if (type == 1) {
      startConcurrentObserverRecv();
    } else {
      startConcurrentPullRecv();
    }
  } else {
    startConcurrentSend();
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  destroyAgoraService();

  return 0;
}
