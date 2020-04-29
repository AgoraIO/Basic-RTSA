#!/bin/bash

target="rtsa"
build_op="rebuild"

while getopts :t:b: opt
do
    case $opt in
        t) target=${OPTARG}
        ;;
        b) build_op=${OPTARG}
        ;;
        \?) echo "Unrecognized opt ..."; exit 1;
    esac
done

if [ ${target} != "rtsa" -a ${target} != "rtc" ]; then
    echo "error target: ${target}"
    exit 1
fi

if [ ${build_op} != "build" -a ${build_op} != "rebuild"  -a ${build_op} != "clean" ]; then
    echo "error build_op: ${build_op}"
    exit 1
fi

if [ ${build_op} == "clean" ]; then
    rm -rf build_linux
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
    rm -rf build_linux
    rm -rf ./third_party/Ogg ./third_party/Opus ./third_party/Opusfile ./third_party/Gtest

    mkdir build_linux
    cd build_linux

    cmake -DCMAKE_BUILD_TYPE=Debug \
        -DTARGET_OS=linux \
        -DTARGET=${target} \
        ..

    make
    exit 0
fi

if [ ${build_op} == "build" ]; then
    cd build_linux
    make
fi

install_demo_dependency() {
    if [ ! -d "../test_data" ];then
        mkdir ../test_data && cd ../test_data
        wget http://114.236.138.34/file/test_data/aac.aac
        wget http://114.236.138.34/file/test_data/ehren-paper_lights-96.opus
        wget http://114.236.138.34/file/test_data/foreman_frames.h
        wget http://114.236.138.34/file/test_data/he_aac.aac
        wget http://114.236.138.34/file/test_data/test.aac
        wget http://114.236.138.34/file/test_data/test.vp8.ivf
        wget http://114.236.138.34/file/test_data/test.wav
        wget http://114.236.138.34/file/test_data/test_multi_slice.h264
    fi
}

# download test data if needed.
install_demo_dependency

exit 0