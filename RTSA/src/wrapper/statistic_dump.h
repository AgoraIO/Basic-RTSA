//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once
#include <stdio.h>
#include <unistd.h>
#include <map>
#include <memory>
#include <mutex>

#include "AgoraBase.h"

struct ThreadTransportStatistic {
  ThreadTransportStatistic()
      : txKBitRateMin(65533), txKBitRateMax(0), lastmileDelayMin(65533), lastmileDelayMax(0) {}

  uint16_t txKBitRateMin;
  uint16_t txKBitRateMax;
  uint16_t lastmileDelayMin;
  uint16_t lastmileDelayMax;
};

class StatisticDump {
 public:
  StatisticDump();
  virtual ~StatisticDump();
  static bool initialize(const char* statisticFilePath);
  static void destroy();
  static void recordRtcStats(pid_t pid, int64_t tid, const agora::rtc::RtcStats& stats);
  static void recordStartThread(int count);
  static void dumpThreadFinalStats(int64_t tid);
  static void dumpFinalStats(pid_t pid);

 private:
  static std::mutex mutex_;
  static FILE* statisticFile_;
  static std::map<int64_t, std::shared_ptr<ThreadTransportStatistic> > threadTransportStatistic_;
};
