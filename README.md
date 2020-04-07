
[English Documents Available](README_en.md)

## 概述

**Agora SDK Demo** 用于演示 **Agora** 声网 **RTC SDK API** 的基本用法，主要包含声网 **RTC SDK** 提供的音频发送、视频发送和视频接收等功能。

## SDK Demo 编译(For android)

切换到 android 目录下，并执行 ndk-build 命令：
```
$ cd AgoraSDKDemo/android
$ ndk-build
```

## SDK Demo 编译(For linux)

**Linux** 下通过执行如下命令进行 **SDK Demo** 编译：

```
$ cd agora_sdk_demo/
$ mkdir build
$ cd build
$ cmake ..
$ make
```

正常结束即可完成编译，产生 **AgoraSDKDemo** 可执行文件。

## SDK Demo运行

#### 参数

**SDK Demo** 支持传递多个参数选项，来控制其行为：

* **-a ：** 用于指定音频发送测试时发送的音频类型，参数值为 **1** 表示发送 **OPUS**，参数值为 **3** 表示发送 **WAV**，参数值为 **7** 表示发送 **AACLC**。默认值为 **1**。
* **-v ：** 用于指定视频发送测试时发送的视频类型，参数值为 **1** 表示发送 **VP8**，参数值为 **2** 表示发送 **H.264**。默认是 **2**。
* **-j ：** 用于指定发送测试时的并发度，即同一时刻起的并发发送音视频流的线程数。默认值为 **1**。
* **-m ：** 用于指定发送测试时发送内容，参数值为 **0** 表示 **音频和视频都不发**，参数值为 **1** 表示 **只发视频**，参数值为 **2** 表示 **只发音频**，参数值为 **3** 表示 **音频和视频都发**。默认值为 **2**。
* **-n ：** 用于指定发送测试的运行轮次。**SDK Demo** 中用于发送测试的音频测试文件或视频测试文件时长为几十秒到几分钟，这个参数用于控制发送这些测试文件的次数。默认值为 **1**。
* **-r ：** 用于指定demo执行接收测试，demo默认执行发送测试，参数为 **0** 表示pull模式接收数据，参数为 **1** 表示 **observer模式** 接收数据。默认值为 **0**。
* **-d ：** 用于指定接收测试时的持续时间，只在 **-r** 时有用。
* **-u ：** 用于指定测试 **userId** ，如果会是多个用户测试，会在该 **userId** 基础上进行变化产生其他的 **userId** 。
* **-c ：** 用于指定测试频道名，默认频道名为 **conn_test_zzz** 。
* **-s ：** 用于observer形式接收时数据，将数据保存到文件，文件格式为 **wav**，recording/playback/mixed 默认文件名是 **user_pcm_audio_data.wav**，before mixed 默认文件名是 **uid + ${uid} + _user_pcm_audio_data.wav**。
    * 参数值为 **0** 表示保存recording数据，即 agora::media::IAudioFrameObserver::onRecordAudioFrame 对应的audio frame（RTSA2.0不支持该模式）
    * 参数值为 **1** 表示保存playback数据，即 agora::media::IAudioFrameObserver::onPlaybackAudioFrame 对应的audio frame
    * 参数值为 **2** 表示保存before mixed数据，即 agora::media::IAudioFrameObserver::onPlaybackAudioFrameBeforeMixing 对应的audio frame
    * 参数值为 **3** 表示保存mixed数据，即 agora::media::IAudioFrameObserver::onMixedAudioFrame 对应的audio frame（RTSA2.0不支持该模式）
* **-p ：** 用于指定音视频以 **Media Packet** 与 **Control Packet** 进行 **Raw data** 的传输，且接收端只能以 **observer** 方式，即 **-p -r 1**。
* **-l ：** 用于使能本地 **audio recorder** ，默认关闭，且 **RTSA2.0** 不支持该功能。

#### 例子

```
$ build/AgoraSDKDemoApp -a 8 -c test_cname     # 单轮单线程发送AAC测试文件，频道名为`test_cname`
$ build/AgoraSDKDemoApp -j 2 -m 1              # 并发2个线程发送视频
$ build/AgoraSDKDemoApp -n 2                   # 进行两次测试
$ build/AgoraSDKDemoApp -r 0 -d 10000          # pull形式接收10秒测试数据，单位毫秒
$ build/AgoraSDKDemoApp -j 10 -u 10000         # 并发10个线程发送音视频，用户Id分别是10000，10001，10002... 10009
$ build/AgoraSDKDemoApp -r 1 -j 5 -d 20000     # 5个用户observer形式接收20秒测试数据，单位毫秒
$ build/AgoraSDKDemoApp -r 1 -s 1              # observer形式接收数据并保存文件，文件名为`user_pcm_audio_data.wav`
```

## 需要使用客户自己appId

```cpp
// AgoraSDKDemo/AgoraSDKWrapper/src/Utils.cpp
#define API_CALL_APPID "use your own appid"  // 替换成自己appId
```
