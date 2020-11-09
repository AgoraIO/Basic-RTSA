
# Agora RTSA-Lite SDK

声网实时码流加速（Real-time Streaming Acceleration, RTSA）Lite版SDK，依托声网自建的底层实时传输网络 Agora SD-RTN™ (Software Defined Real-time Network)，为所有支持网络功能的Linux/RTOS设备，提供音视频码流在互联网实时传输的能力。RTSA充分利用了声网全球全网节点和智能动态路由算法，与此同时支持了前向纠错、智能重传、带宽预测、码流平滑等多种组合抗弱网的策略，可以在设备所处的各种不确定网络环境下，仍然交付高连通、高实时和高稳定的最佳码流传输体验。


由于该SDK具有极小的包体积和内存占用，非常适合运行在典型的IoT设备上。



## 运行示例程序

我们提供了一个简单的示例代码，帮助开发者轻松掌握我们的API使用，从而更高效的集成到自己的应用程序中。该示例代码(hello_rtsa.c)位于example目录，编译后可以演示推流和拉流的基本功能。


### 环境准备

- 一台装有 Ubuntu 16.04 或 18.04 的PC（可以是虚拟机）
- 一台真实 Android 设备


### 创建Agora账号并获取AppId

在编译和启动实例程序前，您需要首先获取一个可用的App ID:
1. 在 [console.agora.io](https://console.agora.io/) 创建一个开发者账号
2. 登录账号后进入主页，点击 **项目列表 > 更多**
3. 创建新的 **App ID** ，并且复制保存，稍后激活License和启动应用时会用到它


### 获取 Agora RESTful API 需要的ID和密钥

1. 在 [console.agora.io/restfulApi](https://console.agora.io/restfulApi/) 页面点击 **添加密钥** 按钮
2. 在下方的页面中会生成新的客户ID和客户密钥，在右边的操作栏点击 **提交** 按钮
3. 页面显示 **创建成功** 提示信息后，在相应的客户密钥栏点击 **下载** 按钮
4. 保存下载下来的 **key_and_secret.txt** ，稍后激活License时会用到它



### hello_rtsa 和 license_activator 编译

```
cd example
./build.sh

编译完成后，会在当前目录里生成 hello_rtsa 和 license_activator 可执行文件。
```


### 激活 License
```
# 修改当前目录下的license配置文件 license.ini
$ vim license.ini

{
    "appId":                "YOUR_OWN_APPID",           // 修改成用户自己创建的App ID或从Agora销售渠道申请的商用App ID
    "customerKey":          "YOUR_CUSTOMER_KEY",        // 修改成`key_and_secret.txt`里的key
    "customerSecret":       "YOUR_CUSTOMER_SECRET",     // 修改成`key_and_secret.txt`里的secret
    "licenseKey":           "YOUR_LICENSE_KEY"          // 修改成用户购买的商用license或免费申请的测试license
}

$ ./license_activator --configFile ./license.ini        // 根据license.ini配置来激活license，并生成相应的证书
```
激活成功后，默认在当前目录生成 **deviceID.bin** 和 **certificate.bin** ，稍后启动应用时会用到它们

### 运行 hello_rtsa

#### 参数

* **-h, --help ：** 打印帮助信息
* **-i, --appId ：** 用于指定用户的appId。无默认值，必填
* **-t, --token ：** 用于指定用户的token。默认值为空字符串
* **-c, --channelId ：** 用于指定加入频道的名称。无默认值，必填
* **-u, --userId ：** 用于指定用户ID。默认值为 **0**，代表SDK随机指定
* **-r, --credentialFile ：** 用于指定设备标识文件。参数为文件路径，默认值为 **./deviceID.bin**
* **-C, --certificateFile ：** 用于指定证书文件。参数为文件路径，默认值为 **./certificate.bin**
* **-a, --audioFile ：** 用于指定推流的音频文件。参数为文件路径，默认值为 **./send_audio.aac**
* **-v, --videoFile ：** 用于指定推流的视频文件。参数为文件路径，默认值为 **./send_video.h264**
* **-A, --audioCodec ：** 用于指定推流的音频编码格式, 默认值为 **8**。（**1**=OPUS，**5**=G722，**8**=AACLC，**9**=HEAAC）
* **-V, --videoCodec ：** 用于指定推流的视频编码格式, 默认值为 **2**。（**1**=VP8，**2**=H264，**6**=GENERIC）
* **-f  --fps ：** 用于指定推流的视频帧率, 默认值为 **30**
* **-l, --sdkLogDir ：** 用于指定存放SDK Log的位置。 参数为目录，默认值为当前目录


#### 示例

可以用下面的命令行，加入名为`hello_channel`的频道，并且用默认参数发送H264视频和AAC音频。
请注意：
- 参数`YOUR_APPID`需要替换为用户自己的appId
- 确保当前目录包含 **deviceID.bin** 和 **certificate.bin**（如果上一个步骤中成功激活了License，应该没有问题）
- 确保当前目录包含 **send_audio.aac** 和 **send_video.h264**（git仓库应该自带这两个测试音频和视频）

```
$ ./hello_rtsa -i YOUR_APPID -c hello_channel
```


## 联系我们

- 如果发现了示例代码的 bug，欢迎提交 [issue](https://github.com/AgoraIO/Basic-RTSA/issues)

