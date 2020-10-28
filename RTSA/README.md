Agora RTSA Quickstart

*English | [中文](README.zh.md)*

## Overview

**Agora SDK Quickstart** is a demo used for demonstrating the basic functions of **Agora RTSA SDK**, which mainly include sending and receiving audio and video.

## Prerequisites
### Development Environment

### Need to use your own app Id

```cpp
// src/wrapper/utils.h
#define API_CALL_APPID "use your own app id" // Replace with your own appId
```

## Compile Quickstart

### Android

```
$ sh ./build_android.sh
# build_android.sh 参数说明
# -t 参数为 rtsa / rtc ，选择构建 RTSA 还是 RTC ，默认为 rtsa
# -a 参数为 armeabi-v7a / arm64-v8a / x86 选择架构，默认为 arm64-v8a
# -v 参数为 Android API Level ，默认为 24
# -b 参数为 build / rebuild / clean ，默认为 rebuild
# -m 开启 sanitizer ，默认不开启

# 需要将 build_android/bin/${arch_abi} 目录 push 到 Android 设备
$ export LD_LIBRARY_PATH=./libs
```

### Linux

Compile **Agora RTSA Quickstart** by executing the following command in Linux:

```
# build_linux.sh 参数 "-b build/rebuild/clean" 默认为 "-b rebuild" 
$ sh ./build_linux.sh
$ cd build_linux/bin/x64
```

Compilation can be completed normally, and the Quickstart executable file is generated.

## Run Quickstart

### Parameters

You can set the following parameters when running **Quickstart** to control its behavior:

* **-a** : Specify the type of audio that the demo sends for audio transmission test:
    * **1**: (Default) **OPUS**.
    * **3**: **WAV**.
    * **7**: **AACLC**.
* **-v** : Specify the type of video that the demo sends for video transmission test. 
    * **1**: **VP8**.
    * **2**: (Default) **H.264**.
* **-j** : Specify the degree of concurrency when sending a test, that is, the number of threads that send audio and video streams concurrently at the same time. The default value is 1.
* **-m** : Specify the content to be sent during the sending test.
    * **0**: Neither audio nor video.
    * **1**: Only video.
    * **2**: (Default) Only audio.
    * **3**: Both audio and video.
* **-n** : Specify the run of the test to be sent. The length of the audio test file or video test file used to send tests in the SDK Demo is tens of seconds to several minutes. This parameter is used to control the number of times these test files are sent. The default value is **1**.
* **-r** : Specify the demo to perform the receiving test. The demo performs the sending test by default. 
    * **0**: (Default) **pull mode** receives data
    * **1**: **observer mode** receive Data. 
* **-b** : Specify the form of sending and receiving DataStream by demo. 
    * **0**: (Default) not open.
    * **1**: **reliable/ordered**.
    * **2**: **unreliable/unordered**.
* **-d** : Used to specify the duration of the reception test, only useful when **-r**.
* **-u** : Used to specify the test userId. If there are multiple user tests, changes will be made based on this userId to generate other userIds.
* **-c** : Used to specify the test channel name. The default channel name is **conn_default_rtsa**.
* **-s ：** Used to receive data in the form of an observer, save the data to a file, the file format is wav. For **recording/playback/mixed** types saving, the default file name is **user_pcm_audio_data.wav**; For **before-mixed** type saving, the default file name is **uid + ${uid} + _user_pcm_audio_data.wav**。
    * **0**: saving **recording** data, i.e. agora::media::IAudioFrameObserver::onRecordAudioFrame receives(**RTSA2.0 not supported**)
    * **1**: saving **playback** data, i.e. agora::media::IAudioFrameObserver::onPlaybackAudioFrame receives
    * **2**: saving **before mixed** data, i.e. agora::media::IAudioFrameObserver::onPlaybackAudioFrameBeforeMixing receives
    * **3**: saving **mixed** data, i.e. agora::media::IAudioFrameObserver::onMixedAudioFrame receives(**RTSA2.0 not supported**)
* **-p** : Used to specify that Raw data is transmitted using Media Packet and Control Packet for audio and video, and the receiver can only use observer mode, that is, **-p -r 1**.

### example

```
$ ./RTSAQuickstart -a 8 -c rtsa           # Send AAC test file in a single round and single thread, the channel name is `rtsa`
$ ./RTSAQuickstart -j 2 -m 1              # Send video concurrently with 2 threads
$ ./RTSAQuickstart -n 2                   # Tested twice
$ ./RTSAQuickstart -r 0 -d 10000          # Receive 10 seconds test data in pull mode, unit is millisecond
$ ./RTSAQuickstart -j 10 -u 10000         # 10 threads send audio and video concurrently, user Id is 10000, 10001, 10002 ... 10009
$ ./RTSAQuickstart -r 1 -j 5 -d 20000     # 5 users receive 20 seconds of test data in the form of an observer, in milliseconds
$ ./RTSAQuickstart -r 1 -s 1              # Receives data in the form of an observer and saves the file with the file name `user_pcm_audio_data.wav.wav`
$ ./RTSAQuickstart -r 2 -b 1              # Send and receive DataStream data in reliable / ordered 
```