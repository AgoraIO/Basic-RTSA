Agora RTSA Quickstart

## 概述

**Agora RTSA Quickstart** 用于演示 **Agora** 声网 **Agora RTSA SDK** 的基本用法，主要包含声网提供的音频发送、视频发送和视频接收等功能。

## 需要使用客户自己appId

```cpp
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

`Quickstart` 支持传递多个参数选项，来控制其行为：

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
$ ./RTSAQuickstart -p -m 1 -c test_cname  # 单轮单线程发送视频media data，频道名为`test_cname`
$ ./RTSAQuickstart -p -j 2 -m 1           # 并发2个线程发送视频media data
$ ./RTSAQuickstart -p -n 2                # 进行两次测试
$ ./RTSAQuickstart -p -r 1                # observer形式接收media data
$ ./RTSAQuickstart -p -r 1 -d 10000       # observer形式接收10秒测试数据，单位毫秒
$ ./RTSAQuickstart -p -j 10 -u 10000      # 并发10个线程发送音视频，用户Id分别是10000，10001，10002... 10009
```