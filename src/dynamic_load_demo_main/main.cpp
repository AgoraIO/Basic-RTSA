//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-08.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <vector>

const char ParseArgsFuncName[] = "parseArgs";
typedef void (*ParseArgsFuncType)(int argc, char* argv[]);

const char InitialFuncName[] = "createAndInitializeAgoraService";
typedef void (*InitialFuncType)(bool, bool, bool);

const char StartSendFuncName[] = "startConcurrentSend";
typedef void (*StartSendFuncType)();

const char StartPullRecvFuncName[] = "startConcurrentPullRecv";
typedef void (*StartPullRecvFuncType)();

const char StartObserverRecvFuncName[] = "startConcurrentObserverRecv";
typedef void (*StartObserverRecvFuncType)();

const char DestroyFuncName[] = "destroyAgoraService";
typedef void (*DestroyFuncType)();

void* loadLibraryAndTransport(int argc, char* argv[], const char* transportLibaryPath, bool recv,
                              int recvType) {
  void* lib = dlopen(transportLibaryPath, RTLD_NOW);

  printf("lib %p\n", lib);
  ParseArgsFuncType parseArgsFunc = (ParseArgsFuncType)dlsym(lib, ParseArgsFuncName);
  parseArgsFunc(argc, argv);

  InitialFuncType initialFunc = (InitialFuncType)dlsym(lib, InitialFuncName);
  StartSendFuncType startSendFunc = (StartSendFuncType)dlsym(lib, StartSendFuncName);
  StartPullRecvFuncType startConcurrentPullRecv =
      (StartPullRecvFuncType)dlsym(lib, StartPullRecvFuncName);
  StartObserverRecvFuncType startConcurrentObserverRecv =
      (StartObserverRecvFuncType)dlsym(lib, StartObserverRecvFuncName);
  DestroyFuncType destroyFunc = (DestroyFuncType)dlsym(lib, DestroyFuncName);

  bool enableAudioDevice = (recv && recvType == 1) ? true : false;
  initialFunc(enableAudioDevice, true, true);

  std::this_thread::sleep_for(std::chrono::milliseconds(739));

  if (recv) {
    if (recvType == 1) {
      startConcurrentObserverRecv();
    } else {
      startConcurrentPullRecv();
    }
  } else {
    startSendFunc();
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  destroyFunc();
  return lib;
}

int main(int argc, char* argv[]) {
  const char* transportLibaryPath = "./build/libAgoraSDKWrapper.so";
  bool recv = false;
  int recvType = 0;
  int ch = 0;
  std::unique_ptr<char* []> tmpagrv(new char*[argc]);
  for (int i = 0; i < argc; ++i) {
    tmpagrv[i] = argv[i];
  }
  while ((ch = getopt(argc, tmpagrv.get(), "p:r:")) != -1) {
    switch (ch) {
      case 'p':
        transportLibaryPath = optarg;
        break;
      case 'r':
        recv = true;
        recvType = atoi(optarg);
        break;
      case '?':
        printf("Unknown option: %c\n", static_cast<char>(optopt));
        break;
    }
  }
  optarg = nullptr;
  optind = 0;
  opterr = 0;
  optopt = 0;

  printf("To load library.\n");
  void* lib = loadLibraryAndTransport(argc, argv, transportLibaryPath, recv, recvType);

  dlclose(lib);

  optarg = nullptr;
  optind = 0;
  opterr = 0;
  optopt = 0;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  printf("To load library again.\n");
  lib = loadLibraryAndTransport(argc, argv, transportLibaryPath, recv, recvType);

  printf("Over!\n");

  return 0;
}
