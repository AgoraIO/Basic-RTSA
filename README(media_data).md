## 概述

`Agora SDK Demo` 用于演示 `Agora` 声网 `RTC SDK API` 的基本用法，主要包含声网 `RTC SDK` 提供的音频发送、视频发送和视频接收等功能。

## SDK Demo 编译(For android)

切换到 android 目录下，并执行 ndk-build 命令：
```
$ cd AgoraSDKDemo/android
$ ndk-build
```

## SDK Demo 编译(For linux)

下通过执行如下命令进行 `SDK Demo` 编译：

```
$ cd AgoraSDKDemo/
$ mkdir build
$ cd build
$ cmake ..
$ make
```

正常结束即可完成编译，产生 `AgoraSDKDemo` 可执行文件。

## SDK Demo运行

#### 参数

`SDK Demo` 支持传递多个参数选项，来控制其行为：

* `-p`：用于指定音视频以Media Packet与Control Packet进行Raw data的传输，且接收端只能以observer方式，即 '-p -r 1'。
* `-j`：用于指定发送测试时的并发度，即同一时刻起的并发发送音视频media data的线程数。默认值为 `1`。
* `-m`：用于指定发送测试时发送内容，参数值为 `0` 表示 `音频和视频media data都不发`，参数值为 `1` 表示 `只发视频media data`，参数值为 `2` 表示 `只发音频media data`，参数值为 `3` 表示 `音频和视频media data都发`。默认值为 `2`。
* `-n`：用于指定发送测试的运行轮次。`SDK Demo` 中用于发送测试的音频或视频media data时长为几十秒到几分钟，这个参数用于控制发送这些测试数据的次数。默认值为 `1`。
* `-r`：用于指定demo执行接收测试，demo默认执行发送测试，参数为 `0` 表示pull模式接收数据，参数为 `1` 表示observer模式接收数据，media data只能以observer模式接收数据。默认值为 `0`。
* `-d`：用于指定接收测试时的持续时间，只在`-r`时有用。
* `-u`：用于指定测试userId，如果会是多个用户测试，会在该userId基础上进行变化产生其他的userId。
* `-c`：用于指定测试频道名，默认频道名为`conn_test_zzz`。

#### 例子

```
$ build/AgoraSDKDemoApp -p -m 1 -c test_cname  # 单轮单线程发送视频media data，频道名为`test_cname`
$ build/AgoraSDKDemoApp -p -j 2 -m 1           # 并发2个线程发送视频media data
$ build/AgoraSDKDemoApp -p -n 2                # 进行两次测试
$ build/AgoraSDKDemoApp -p -r 1                # observer形式接收media data
$ build/AgoraSDKDemoApp -p -r 1 -d 10000       # observer形式接收10秒测试数据，单位毫秒
$ build/AgoraSDKDemoApp -p -j 10 -u 10000      # 并发10个线程发送音视频，用户Id分别是10000，10001，10002... 10009
```

## 需要使用客户自己appId

```cpp
// AgoraSDKDemo/AgoraSDKWrapper/src/Utils.cpp
#define API_CALL_APPID "use your own appid" //替换成自己appId
```
