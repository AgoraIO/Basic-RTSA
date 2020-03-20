//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//
#include <string.h>

#include "statistic_dump.h"

std::mutex StatisticDump::mutex_;
FILE* StatisticDump::statisticFile_ = nullptr;
std::map<int64_t, std::shared_ptr<ThreadTransportStatistic> >
    StatisticDump::threadTransportStatistic_;

StatisticDump::StatisticDump() {}

StatisticDump::~StatisticDump() {}

bool StatisticDump::initialize(const char* statisticFilePath) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (statisticFile_) {
    printf("StatisticDump has been initialized.\n");
    return true;
  }

  statisticFile_ = fopen(statisticFilePath, "w+");
  if (statisticFile_) {
    constexpr static int LogBufferSize = 512;
    char buffer[LogBufferSize] = {0};
    snprintf(buffer, LogBufferSize,
             "%-s\n%-s\n%-s\n%-s\n%-s\n%-s\n%-s\n%-s\n%-s\n%-s\n%-s\n%-s\n%-s\n\n", "duration(s),",
             "txBytes,", "rxBytes,", "txKBitRate(kbit/s),", "rxKBitRate(kbit/s),",
             "rxAudioKBitRate(kbit/s),", "txAudioKBitRate(kbit/s),", "rxVideoKBitRate(kbit/s),",
             "txVideoKBitRate(kbit/s),", "lastmileDelay(ms),", "userCount,", "cpuAppUsage,",
             "cpuTotalUsage");
    fwrite(buffer, 1, strlen(buffer), statisticFile_);
    fflush(statisticFile_);
  }
  return statisticFile_ != nullptr;
}

void StatisticDump::destroy() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (statisticFile_) {
    fclose(statisticFile_);
    statisticFile_ = nullptr;
  }
}

void StatisticDump::recordRtcStats(pid_t pid, int64_t tid, const agora::rtc::RtcStats& stats) {
  if (!statisticFile_) {
    return;
  }
  constexpr static int LogBufferSize = 512;
  char buffer[LogBufferSize] = {0};
  bool updated = true;
  std::lock_guard<std::mutex> lock(mutex_);
  if (threadTransportStatistic_.find(tid) == threadTransportStatistic_.end()) {
    std::shared_ptr<ThreadTransportStatistic> statistic(new ThreadTransportStatistic);
    if (stats.txAudioKBitRate != 0) {
      statistic->txKBitRateMin = stats.txAudioKBitRate;
      statistic->txKBitRateMax = stats.txAudioKBitRate;
    }
    if (stats.lastmileDelay != 0) {
      statistic->lastmileDelayMin = stats.lastmileDelay;
      statistic->lastmileDelayMax = stats.lastmileDelay;
    }
    threadTransportStatistic_[tid] = statistic;
  } else {
    updated = false;
    std::shared_ptr<ThreadTransportStatistic> statistic = threadTransportStatistic_[tid];
    if (stats.txAudioKBitRate != 0 && stats.txAudioKBitRate > statistic->txKBitRateMax) {
      statistic->txKBitRateMax = stats.txAudioKBitRate;
      updated = true;
    } else if (stats.txAudioKBitRate != 0 && stats.txAudioKBitRate < statistic->txKBitRateMin) {
      statistic->txKBitRateMin = stats.txAudioKBitRate;
      updated = true;
    }
    if (stats.lastmileDelay != 0 && stats.lastmileDelay > statistic->lastmileDelayMax) {
      statistic->lastmileDelayMax = stats.lastmileDelay;
      updated = true;
    } else if (stats.lastmileDelay != 0 && stats.lastmileDelay < statistic->lastmileDelayMin) {
      statistic->lastmileDelayMin = stats.lastmileDelay;
      updated = true;
    }
  }
  if (statisticFile_ && updated) {
    snprintf(buffer, LogBufferSize,
             "%ld,%d,%ld[%-u, %-u, %-u, %-u, %-u, %-u, %-u, %-u, %-u, %-u, %-u, %-f, %-f, %-f]\n",
             time(nullptr), pid, tid, stats.duration, stats.txBytes, stats.rxBytes,
             stats.txKBitRate, stats.rxKBitRate, stats.rxAudioKBitRate, stats.txAudioKBitRate,
             stats.rxVideoKBitRate, stats.txVideoKBitRate, stats.lastmileDelay, stats.userCount,
             stats.cpuAppUsage, stats.cpuTotalUsage,
             (stats.txBytes / static_cast<float>(stats.duration)));
    fwrite(buffer, 1, strlen(buffer), statisticFile_);
    fflush(statisticFile_);
  }
}

void StatisticDump::recordStartThread(int count) {
  if (!statisticFile_) {
    return;
  }
  constexpr static int LogBufferSize = 512;
  char buffer[LogBufferSize] = {0};
  snprintf(buffer, LogBufferSize, "Start thread %d\n", count);
  fwrite(buffer, 1, strlen(buffer), statisticFile_);
  fflush(statisticFile_);
}

void StatisticDump::dumpThreadFinalStats(int64_t tid) {
  if (!statisticFile_) {
    return;
  }
  constexpr static int LogBufferSize = 512;
  char buffer[LogBufferSize] = {0};
  std::lock_guard<std::mutex> lock(mutex_);
  if (threadTransportStatistic_.find(tid) != threadTransportStatistic_.end()) {
    std::shared_ptr<ThreadTransportStatistic> statistic = threadTransportStatistic_[tid];
    snprintf(buffer, LogBufferSize, "%d,%ld[%-u, %-u, %-u, %-u]\n", getpid(), tid,
             statistic->txKBitRateMin, statistic->txKBitRateMax, statistic->lastmileDelayMin,
             statistic->lastmileDelayMax);
    fwrite(buffer, 1, strlen(buffer), statisticFile_);
  }
  fflush(statisticFile_);
}

void StatisticDump::dumpFinalStats(pid_t pid) {
  if (!statisticFile_) {
    return;
  }
  constexpr static int LogBufferSize = 512;
  char buffer[LogBufferSize] = {0};
  std::shared_ptr<ThreadTransportStatistic> statisticSum(new ThreadTransportStatistic);

  std::lock_guard<std::mutex> lock(mutex_);
  for (auto& statistic : threadTransportStatistic_) {
    if (statistic.second->txKBitRateMax > statisticSum->txKBitRateMax) {
      statisticSum->txKBitRateMax = statistic.second->txKBitRateMax;
    }
    if (statistic.second->txKBitRateMin != 0 &&
        statistic.second->txKBitRateMin < statisticSum->txKBitRateMin) {
      statisticSum->txKBitRateMin = statistic.second->txKBitRateMin;
    }
    if (statistic.second->lastmileDelayMax > statisticSum->lastmileDelayMax) {
      statisticSum->lastmileDelayMax = statistic.second->lastmileDelayMax;
    }
    if (statistic.second->lastmileDelayMin != 0 &&
        statistic.second->lastmileDelayMin < statisticSum->lastmileDelayMin) {
      statisticSum->lastmileDelayMin = statistic.second->lastmileDelayMin;
    }
  }
  fwrite("Process summary: \n", 1, strlen("Process summary: \n"), statisticFile_);
  snprintf(buffer, LogBufferSize, "%d[%-u, %-u, %-u, %-u]\n", pid, statisticSum->txKBitRateMin,
           statisticSum->txKBitRateMax, statisticSum->lastmileDelayMin,
           statisticSum->lastmileDelayMax);
  fwrite(buffer, 1, strlen(buffer), statisticFile_);
  fflush(statisticFile_);
}
