
## 概述

**Agora Server SDK Demo** 用于演示 **Agora** 声网 **RTC SDK API** 的在Linux Server端的基本用法，目前包含了声网 **RTC SDK API** 提供的音频、视频裸数据发送功能。

## Server SDK Demo 编译 (For Linux)

**Linux** 下通过执行如下命令进行所有 **SDK Demo** 的编译：

```
$ cd agora_sdk_demo/
$ mkdir build
$ cd build
$ cmake ..
$ make AgoraServerSdkDemoApp
```

正常结束即可完成编译，产生 **AgoraServerSdkDemoApp** 可执行文件。

## Server SDK Demo运行

#### 参数

**Server SDK Demo** 支持传递多个参数选项，来控制其行为：

* **--appId ：** 用于指定用户的appId。无默认值，必填。
* **--channelId ：** 用于指定加入频道的名称。无默认值，必填。
* **--userId ：** 用于指定用户ID。默认值为 **0**。
* **--audioFile ：** 用于指定发送的音频文件（目前只支持PCM格式），参数值为文件路径。默认文件为 **test_data/right_48khz.pcm**
* **--videoFile ：** 用于指定发送的视频文件（目前只支持YUV420格式），参数值为文件路径。默认文件为 **test_data/video.yuv**
* **--width ：** 用于指定发送的视频的像素宽度。默认值为 **640**
* **--height ：** 用于指定发送的视频的像素高度。默认值为 **360**
* **--bitrate ：** 用于指定发送的视频码率。默认值为 **1000000**

#### 例子

```
# 发送pcm和yuv裸数据，频道名为`test_cname`，视频分辨率为`640x360`，视频目标码率为`500kbps`，参数`XXXXXX`需要替换为用户自己的appId
$ build/AgoraServerSdkDemoApp --addId XXXXXX --channelId test_cname --uid 123456 --audioFile test_audio.pcm --videoFile test_video.yuv --width 640 --height 360 --bitrate 500000
```