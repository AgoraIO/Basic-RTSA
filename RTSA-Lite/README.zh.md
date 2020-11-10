
# Agora RTSA-Lite SDK

声网实时码流加速（Real-time Streaming Acceleration, RTSA）SDK Lite 版，依托声网自建的底层实时传输网络 Agora SD-RTN™ (Software Defined Real-time Network)，为所有支持网络功能的 Linux/RTOS 设备提供音视频码流在互联网实时传输的能力。RTSA 充分利用了声网全球全网节点和智能动态路由算法，与此同时支持了前向纠错、智能重传、带宽预测、码流平滑等多种组合抗弱网的策略，可以在设备所处的各种不确定网络环境下，仍然交付高连通、高实时和高稳定的最佳码流传输体验。此外，该 SDK 具有极小的包体积和内存占用，非常适合运行在典型的 IoT 设备上。

## 运行示例项目

声网提供了一个简单的示例项目，帮助开发者轻松掌握我们的API使用，从而更高效的集成到自己的应用程序中。该示例项目 hello_rtsa.c 位于 example 目录，编译后可以演示推流和拉流的基本功能。

### 环境准备

- 一台装有 Ubuntu 16.04 或 18.04 的 PC，可以是虚拟机。
- 一台真实 Android 设备。

### 操作步骤

#### 1. 创建 Agora 账号并获取 App ID

在编译和运行示例项目前，你首先需要通过以下步骤获取 Agora App ID:
1. 创建一个有效的 [Agora 账号](https://console.agora.io/)。
2. 登录 [Agora 控制台](https://console.agora.io/)，点击左侧导航栏项目管理按钮进入项目管理页面。
3. 在项目管理页面，点击创建按钮。在弹出的对话框内输入项目名称，选择鉴权机制为 APP ID + Token。点击提交，新建的项目就会显示在项目管理页中。Agora 会给每个项目自动分配一个 App ID 作为项目唯一标识。复制并保存此项目的 **App ID** ，稍后激活 License 和启动应用时会用到 App ID。

### 获取 Agora RESTful API 需要的客户 ID 和客户密钥
由于该示例项目需要使用 Agora RESTful API，因此你需要进行 HTTP 基本认证，即使用 Agora 提供的客户 ID 和客户密钥生成一个 Authorization 字段（使用 Base64 算法编码）。
1. 登录 [Agora 控制台](https://console.agora.io/)，点击页面右上角的用户名，在下拉列表中打开 RESTful API 页面。
2. 点击 **添加密钥** 按钮。在下方的页面中会生成新的客户 ID 和客户密钥，在右边的操作栏点击**提交**按钮。
3. 页面显示**创建成功**提示信息后，在相应的客户密钥栏点击**下载**按钮。
4. 保存下载下来的 **key_and_secret.txt** ，里面包含客户 ID 和客户密钥，稍后激活 License 时会用到。

### 编译 hello_rtsa 和 license_activator

通过以下命令编译 hello_rtsa 和 license_activator。
```
cd example
./build.sh
```
编译完成后，会在当前目录里生成 hello_rtsa 和 license_activator 可执行文件。

### 激活 License
通过以下步骤激活 License:
1. 修改当前目录下的 License 配置文件 license.ini。
```
$ vim license.ini
{
    "appId":                "YOUR_OWN_APPID",           // 修改成你创建的 App ID 或从 Agora 销售渠道申请的商用 App ID
    "customerKey":          "YOUR_CUSTOMER_KEY",        // 修改成 `key_and_secret.txt` 里的 key
    "customerSecret":       "YOUR_CUSTOMER_SECRET",     // 修改成 `key_and_secret.txt` 里的 secret
    "licenseKey":           "YOUR_LICENSE_KEY"          // 修改成你购买的商用 license 或免费申请的测试 license
}
```
2. 根据修改后的 license.ini 配置来激活 license，并生成相应的证书。
```
$ ./license_activator --configFile ./license.ini
```
激活成功后，默认在当前目录生成 **deviceID.bin** 和 **certificate.bin** ，稍后启动应用时会用到它们。

### 运行 hello_rtsa

#### 参数

* **-h, --help ：** 打印帮助信息。
* **-i, --appId ：** 用于指定用户的appId。无默认值，必填。
* **-t, --token ：** 用于指定用户的token。默认值为空字符串。
* **-c, --channelId ：** 用于指定加入频道的名称。无默认值，必填。
* **-u, --userId ：** 用于指定用户ID。默认值为 **0**，代表 SDK 随机指定。
* **-r, --credentialFile ：** 用于指定设备标识文件。参数为文件路径，默认值为 **./deviceID.bin**。
* **-C, --certificateFile ：** 用于指定证书文件。参数为文件路径，默认值为 **./certificate.bin**。
* **-a, --audioFile ：** 用于指定推流的音频文件。参数为文件路径，默认值为 **./send_audio.aac**。
* **-v, --videoFile ：** 用于指定推流的视频文件。参数为文件路径，默认值为 **./send_video.h264**。
* **-A, --audioCodec ：** 用于指定推流的音频编码格式, 默认值为 **8**。
  * **1**: OPUS
  * **5**: G722
  * **8**: AACLC
  * **9**: HEAAC
* **-V, --videoCodec ：** 用于指定推流的视频编码格式, 默认值为 **2**。
  * **1**: VP8
  * **2**: H264
  * **6**: GENERIC
* **-f  --fps ：** 用于指定推流的视频帧率, 默认值为 **30**。
* **-l, --sdkLogDir ：** 用于指定存放SDK Log的位置。 参数为目录，默认值为当前目录。


#### 示例

可以用下面的命令行，加入名为 `hello_channel` 的频道，并且用默认参数发送 H.264 视频和 AAC 音频。
```
$ ./hello_rtsa -i YOUR_APPID -c hello_channel
```

请注意：
- 参数 `YOUR_APPID` 需要替换为你自己的 App ID。
- 确保当前目录包含 **deviceID.bin** 和 **certificate.bin**（如果上一个步骤中成功激活了 License，应该没有问题）。
- 确保当前目录包含 **send_audio.aac** 和 **send_video.h264**（git 仓库自带这两个测试音频和视频）。

## 联系我们

- 如果发现了示例代码的 bug，欢迎提交 [issue](https://github.com/AgoraIO/Basic-RTSA/issues)

