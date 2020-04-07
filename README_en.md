
[中文](README.md)

## Overview

**Agora SDK Demo** is used to demonstrate the basic usage of **Agora** sound network **RTC SDK API** , which mainly includes audio sending, video sending and video receiving functions provided by sound network **RTC SDK**.

## SDK Demo Compile(For android)

Change to the android directory and execute the ndk-build command:
```
$ cd AgoraSDKDemo/android
$ ndk-build
```

## SDK Demo Compile(For linux)

Compile **SDK Demo** by executing the following command in Linux:

```
$ cd AgoraSDKDemo/
$ mkdir build
$ cd build
$ cmake ..
$ make
```

Compilation can be completed normally, and the AgoraSDKDemo executable file is generated.

## SDK Demo Run

#### parameter

**SDK Demo** supports passing multiple parameter options to control its behavior:

* **-a** : Used to specify the type of audio sent during the audio transmission test. A parameter value of **1** indicates that **OPUS** is sent, a parameter value of **3** indicates that **WAV** is sent, and a parameter value of **7** indicates that **AACLC** is sent. The default value is **1**.

* **-v** : Used to specify the type of video sent during the video sending test. A parameter value of **1** indicates that **VP8** is sent, and a parameter value of **2** indicates that **H.264** is sent. The default is 2.

* **-j** : Used to specify the degree of concurrency when sending a test, that is, the number of threads that send audio and video streams concurrently from the same moment. The default value is 1.

* **-m** : Used to specify the content to be sent during the sending test. A parameter value of **0** indicates that **neither audio nor video is sent**, a parameter value of **1** indicates that **only video is sent**, a parameter value of **2** indicates that **only audio is sent**, and a parameter value of **3** indicates that **both audio and video are sent**. The default value is 2.

* **-n** : Used to specify the run of the test to be sent. The length of the audio test file or video test file used to send tests in the SDK Demo is tens of seconds to several minutes. This parameter is used to control the number of times these test files are sent. The default value is **1**.

* **-r** : Used to specify the demo to perform the receiving test. The demo performs the sending test by default. A parameter of **0** indicates **receiving data in pull mode**, and a parameter of **1** indicates **receiving data in observer mode**. The default value is 0.

* **-d** : Used to specify the duration of the reception test, only useful when **-r**.

* **-u** : Used to specify the test userId. If there are multiple user tests, changes will be made based on this userId to generate other userIds.

* **-c** : Used to specify the test channel name. The default channel name is **conn_test_zzz**.

* **-s ：** Used to receive data in the form of an observer, save the data to a file, the file format is wav. For **recording/playback/mixed** types saving, the default file name is **user_pcm_audio_data.wav**; For **before-mixed** type saving, the default file name is **uid + ${uid} + _user_pcm_audio_data.wav**。
    * Parameter **0** means **recording** type, i.e. agora::media::IAudioFrameObserver::onRecordAudioFrame receives(**RTSA2.0 not supported**)
    * Parameter **1** means **playback** type, i.e. agora::media::IAudioFrameObserver::onPlaybackAudioFrame receives
    * Parameter **2** means **before mixed** type, i.e. agora::media::IAudioFrameObserver::onPlaybackAudioFrameBeforeMixing receives
    * Parameter **3** means **mixed**, i.e. agora::media::IAudioFrameObserver::onMixedAudioFrame receives(**RTSA2.0 not supported**)

* **-p** : Used to specify that Raw data is transmitted using Media Packet and Control Packet for audio and video, and the receiver can only use observer mode, that is, **-p -r 1**.

* **-l** : Used to enable the local audio recorder. It is disabled by default, and RTSA 2.0 does not support this function.

#### example

```
$ build/AgoraSDKDemoApp -a 8 -c test_cname     # Send AAC test file in a single round and single thread, the channel name is `test_cname`
$ build/AgoraSDKDemoApp -j 2 -m 1              # Send video concurrently with 2 threads
$ build/AgoraSDKDemoApp -n 2                   # Tested twice
$ build/AgoraSDKDemoApp -r 0 -d 10000          # Receive 10 seconds test data in pull mode, unit is millisecond
$ build/AgoraSDKDemoApp -j 10 -u 10000         # 10 threads send audio and video concurrently, user Id is 10000, 10001, 10002 ... 10009
$ build/AgoraSDKDemoApp -r 1 -j 5 -d 20000     # 5 users receive 20 seconds of test data in the form of an observer, in milliseconds
$ build/AgoraSDKDemoApp -r 1 -s 1              # Receives data in the form of an observer and saves the file with the file name `user_pcm_audio_data.wav.wav`
```

## Need to use your own appId

```cpp
// AgoraSDKDemo/AgoraSDKWrapper/src/Utils.cpp
#define API_CALL_APPID "use your own appid" // Replace with your own appId
```
