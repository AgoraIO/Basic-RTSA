Agora RTSA Quickstart

*[English](README.md) | 中文*

## 概述

**Agora SDK Quickstart** 用于演示 **Agora** 声网 **Agora RTSA SDK** 的基本用法，主要包含声网提供的音频发送、视频发送和视频接收等功能。


## 需要使用客户自己的 app Id

```
// src/wrapper/utils.h
#define API_CALL_APPID "use your own app id"  // 替换成自己 app Id
```

## Agora RTSA Quickstart 编译(For Android)

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

## Agora RTSA Quickstart 编译(For Linux)

**Linux** 下通过执行如下命令进行 **Agora RTSA Quickstart** 编译：

```
# build_linux.sh 参数 "-b build/rebuild/clean" 默认为 "-b rebuild" 
$ sh ./build_linux.sh
$ cd build_linux/bin/x64
```

正常结束即可完成编译，产生 **Quickstart** 可执行文件。

## Quickstart 运行

#### 参数

**Quickstart** 支持传递多个参数选项，来控制其行为：

* **-a ：** 用于指定音频发送测试时发送的音频类型。
    * **1**: (默认值) 表示发送 **OPUS**。
    * **3**: 表示发送 **WAV**。
    * **7**: 表示发送 **AACLC**。
* **-v ：** 用于指定视频发送测试时发送的视频类型。
    * **1**: 表示发送 **VP8**。
    * **2**: (默认值) 表示发送 **H.264**。
* **-j ：** 用于指定发送测试时的并发度，即同一时刻起的并发发送音视频流的线程数。默认值为 **1**。
* **-m ：** 用于指定发送测试时发送内容。
    * **0**: 表示 **音频和视频都不发**。
    * **1**: 表示 **只发视频**。
    * **2**: (默认值) 表示 **只发音频**。
    * **3**: 表示 **音频和视频都发**。
* **-n ：** 用于指定发送测试的运行轮次。**SDK Demo** 中用于发送测试的音频测试文件或视频测试文件时长为几十秒到几分钟，这个参数用于控制发送这些测试文件的次数。默认值为 **1**。
* **-r ：** 用于指定 demo 执行接收测试，demo 默认执行发送测试。
    * **0**: (默认值) 表示 **pull模式** 接收数据。
    * **1**: 表示 **observer模式** 接收数据。
* **-b ：** 用于指定 demo 收发 DataStream 的形式。
    * **0**: (默认值) 不开启。
    * **1**: 表示 **reliable/ordered**。
    * **2**: 表示 **unreliable/unordered**。
* **-d ：** 用于指定接收测试时的持续时间，只在 **-r** 时有用。
* **-u ：** 用于指定测试 **userId** ，如果会是多个用户测试，会在该 **userId** 基础上进行变化产生其他的 **userId** 。
* **-c ：** 用于指定测试频道名，默认频道名为 **conn_default_rtsa** 。
* **-s ：** 用于 observer 形式接收时数据，将数据保存到文件，文件格式为 **wav**，recording/playback/mixed 默认文件名是 **user_pcm_audio_data.wav**，before mixed 默认文件名是 **uid + ${uid} + _user_pcm_audio_data.wav**。
    * 参数值为 **0** 表示保存 recording 数据，即 agora::media::IAudioFrameObserver::onRecordAudioFrame 对应的 audio frame（RTSA2.0不支持该模式）。
    * 参数值为 **1** 表示保存 playback 数据，即 agora::media::IAudioFrameObserver::onPlaybackAudioFrame 对应的 audio frame。
    * 参数值为 **2** 表示保存 before mixed数据，即 agora::media::IAudioFrameObserver::onPlaybackAudioFrameBeforeMixing 对应的 audio frame。
    * 参数值为 **3** 表示保存 mixed 数据，即 agora::media::IAudioFrameObserver::onMixedAudioFrame 对应的 audio frame（RTSA2.0不支持该模式）。
* **-p ：** 用于指定音视频以 **Media Packet** 与 **Control Packet** 进行 **Raw data** 的传输，且接收端只能以 **observer** 方式，即 **-p -r 1**。

#### 例子

```
$ ./RTSAQuickstart -a 8 -c rtsa           # 单轮单线程发送 AAC 测试文件，频道名为`rtsa`
$ ./RTSAQuickstart -j 2 -m 1              # 并发 2 个线程发送视频
$ ./RTSAQuickstart -n 2                   # 进行两次测试
$ ./RTSAQuickstart -r 0 -d 10000          # pull 形式接收 10 秒测试数据，单位毫秒
$ ./RTSAQuickstart -j 10 -u 10000         # 并发 10 个线程发送音视频，用户 Id 分别是 10000，10001，10002... 10009
$ ./RTSAQuickstart -r 1 -j 5 -d 20000     # 5 个用户 observer 形式接收 20 秒测试数据，单位毫秒
$ ./RTSAQuickstart -r 1 -s 1              # observer 形式接收数据并保存文件，文件名为 `user_pcm_audio_data.wav`
$ ./RTSAQuickstart -b 1                   # 以 reliable/ordered 发送 DataStream 数据
```