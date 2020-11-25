# 交叉编译指南

## 1. 修改 example/toolchain.cmake，设置交叉编译工具链的路径和前缀

比如，你使用的工具链是ARM官方的GNU Toolchain，则可以这样设置
```
set(DIR /home/username/toolchain/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/)
set(CROSS "arm-linux-gnueabihf-")
```

注意：工具链的位置必须设置为 **绝对路径**

## 2. 选择一个合适的SDK库
根据目标设备的操作系统、芯片架构和工具链类型，选择相应的SDK压缩包下载到本地，解压，然后把其中的 **libagora-rtc-sdk.so** 复制到 **agora_sdk/lib/** 目录，替换原来的库文件

[aarch64-linux-gnu](https://download.agora.io/rtsasdk/release/AGORA-RTSALite-license-aarch64-linux-gnu.tgz)<br>
[aarch64-linux-musl](https://download.agora.io/rtsasdk/release/AGORA-RTSALite-license-aarch64-linux-musl.tgz)<br>
[arm-linux-gnueabi](https://download.agora.io/rtsasdk/release/AGORA-RTSALite-license-arm-linux-gnueabi.tgz)<br>
[arm-linux-gnueabihf](https://download.agora.io/rtsasdk/release/AGORA-RTSALite-license-arm-linux-gnueabihf.tgz)<br>
[arm-linux-uclibceabi](https://download.agora.io/rtsasdk/release/AGORA-RTSALite-license-arm-linux-uclibceabi.tgz)<br>
[arm-linux-uclibceabihf](https://download.agora.io/rtsasdk/release/AGORA-RTSALite-license-arm-linux-uclibceabihf.tgz)<br>
[arm-linux-musleabi](https://download.agora.io/rtsasdk/release/AGORA-RTSALite-license-arm-linux-musleabi.tgz)<br>
[arm-linux-musleabihf](https://download.agora.io/rtsasdk/release/AGORA-RTSALite-license-arm-linux-musleabihf.tgz)<br>
[arm-liteos-eabi](https://download.agora.io/rtsasdk/release/AGORA-RTSALite-license-arm-liteos-eabi.tgz)<br>
[arm11-linux-uclibceabi](https://download.agora.io/rtsasdk/release/AGORA-RTSALite-license-arm11-linux-uclibceabi.tgz)<br>
[arm11-linux-uclibceabihf](https://download.agora.io/rtsasdk/release/AGORA-RTSALite-license-arm11-linux-uclibceabihf.tgz)<br>
[mips-linux-uclibceabihf](https://download.agora.io/rtsasdk/release/AGORA-RTSALite-license-mips-linux-uclibceabihf.tgz)<br>

## 3. 编译：
```
cd example
./build.sh
```
