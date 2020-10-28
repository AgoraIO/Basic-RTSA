#!/bin/bash

target="rtsa"
arch_abi="arm64-v8a"
abi_version=24
build_op="rebuild"
sanitizer="false"

while getopts :t:a:v:b:m opt
do
    case $opt in
        t) target=${OPTARG}
        ;;
        a) arch_abi=${OPTARG}
        ;;
        v) abi_version=${OPTARG}
        ;;
        b) build_op=${OPTARG}
        ;;
        m) sanitizer="true"
        ;;
        \?) echo "Unrecognized opt ..."; exit 1;
    esac
done
echo -e "\033[0;32m --> target: ${target}, arch_abi: ${arch_abi}, abi_version: ${abi_version}, build_op: ${build_op}, sanitizer: ${sanitizer}\033[0;0m"

if [ ${target} != "rtsa" -a ${target} != "rtc" ]; then
    echo "error target: ${target}"
    exit 1
fi

if [ ${target} == "rtc" ]; then
    echo "android rtc is not supported for this demo"
    exit 1
fi

if [ ${arch_abi} != "armeabi-v7a" -a ${arch_abi} != "arm64-v8a" -a ${arch_abi} != "x86" ]; then
    echo "error arch_abi: ${arch_abi}"
    exit 1
fi

if [ ${build_op} != "build" -a ${build_op} != "rebuild"  -a ${build_op} != "clean" ]; then
    echo "error build_op: ${build_op}"
    exit 1
fi

if [ ${build_op} == "clean" ]; then
    rm -rf build_android
    rm -rf ./third_party/Ogg ./third_party/Opus ./third_party/Opusfile ./third_party/Gtest
    exit 0
fi

install_demo_dependency() {
    if [ ! -d "$1/test_data" ];then
        mkdir -p "$1/test_data" && cd "$1/test_data"
        wget http://114.236.138.34/file/test_data/aac.aac
        wget http://114.236.138.34/file/test_data/ehren-paper_lights-96.opus
        wget http://114.236.138.34/file/test_data/foreman_frames.h
        wget http://114.236.138.34/file/test_data/he_aac.aac
        wget http://114.236.138.34/file/test_data/test.aac
        wget http://114.236.138.34/file/test_data/test.vp8.ivf
        wget http://114.236.138.34/file/test_data/test.wav
        wget http://114.236.138.34/file/test_data/test_multi_slice.h264
        cd -
    fi
}

# download test data if needed.
install_demo_dependency "."

if [ ${build_op} == "rebuild" ]; then
    rm -rf build_android
    rm -rf ./third_party/Ogg ./third_party/Opus ./third_party/Opusfile ./third_party/Gtest

    mkdir build_android
    cd build_android

    cmake -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
        -DANDROID_NDK=$ANDROID_NDK \
        -DANDROID_ABI=${arch_abi} \
        -DANDROID_NATIVE_API_LEVEL=${abi_version} \
        -DANDROID_STL=c++_static \
        -DTARGET=${target} \
        -DTARGET_OS=android \
        -DMEM_CHECK=${sanitizer} \
        ..

    make
fi

if [ ${build_op} == "build" ]; then
    cd build_android
    make
fi

if [ ! -d "./bin/${arch_abi}/libs/" ];then
    mkdir -p ./bin/${arch_abi}/libs/
fi

if [ ${target} == "rtsa" ]; then
    lib_sdk=libagora-rtsa-sdk.so
else
    lib_sdk=libagora-rtc-sdk-jni.so
fi

packetizer_libs() {
    cp ./Ogg-build/libogg.so ./bin/${arch_abi}/libs/
    cp ./Opus-build/libopus.so ./bin/${arch_abi}/libs/
    cp ./Opusfile-build/libopusfile.so ./bin/${arch_abi}/libs/
    cp ./lib/* ./bin/${arch_abi}/libs/
    cp ../../agora_rtsa_sdk/libs/${arch_abi}/${lib_sdk} ./bin/${arch_abi}/libs/
    mv ./bin/${arch_abi}/libAgoraSDKWrapper.so ./bin/${arch_abi}/libs/
    cp -r ../test_data ./bin/${arch_abi}/
}

# archive all .so together for convenience
packetizer_libs

exit 0