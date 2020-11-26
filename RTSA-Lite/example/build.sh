#!/bin/bash

build_op="build"
target_os="linux"
build_type="release"
arch_abi="arm64-v8a"
ANDROID_NDK=${ANDROID_NDK_ROOT}
license="on"

# to clean
build_files=("build" "hello_rtsa" "license_activator" "license_query")

export cur=$(dirname $(readlink -f $0))

function cmake_build() {
    src=$(readlink -f $1)
    shift
    if [ -d build ]; then
        rm -rf build
    fi

    if [ ${build_type} == "debug" ]; then
        debug_flag="-DCMAKE_BUILD_TYPE=Debug "
    elif [ ${build_type} = "asan" ]; then
        debug_flag="-DCMAKE_BUILD_TYPE=asan "
    fi

    if [ ${target_os} == "linux" ]; then
        echo "target OS is Linux"
        os_flag="-DCMAKE_TOOLCHAIN_FILE=$cur/toolchain.cmake"
    elif [ ${target_os} = "android" ]; then
        echo "target OS is Android"
        os_flag="-DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
        -DANDROID_ABI=${arch_abi}"
    fi

    mkdir build && cd build &&
        cmake $src $debug_flag $os_flag -DSDK_LICENSE_ENABLE=${license} $* &&
        make -j8 || return 1
    cd -
}

function clean_build_files() {
    for file in ${build_files[@]}; do
        if [ -d ${file} ]; then
            rm -rf ${file}
        elif [ -f ${file} ]; then
            rm ${file}
        fi
    done

}

function help() {
    echo -e "$0 [-b <build_op>] [-l <license>] [-t <type>]"
    echo -e "\t -b <build_op>, build|clean|rebuild, default: build"
    echo -e "\t -l <license>, on|off, default: off"
    echo -e "\t -t <type>, release|debug, default: release"
}

while getopts 'b:l:o:t:h' opt; do
    case $opt in
    b)
        build_op=${OPTARG}
        ;;
    l)
        license=${OPTARG}
        ;;
    o)
        target_os=${OPTARG}
        ;;
    t)
        build_type=${OPTARG}
        ;;
    h)
        help
        exit 1
        ;;
    \?)
        help
        exit 1
        ;;
    esac
done

if [ ${build_op} != "build" -a ${build_op} != "rebuild" -a ${build_op} != "clean" ]; then
    echo "error build_op: ${build_op}"
    exit 1
fi

if [ ${license} != "on" -a ${license} != "off" ]; then
    echo "error license: ${license}"
    exit 1
fi

if [ ${build_op} = "build" ]; then
    echo "building ..."
    cmake_build .
    echo "build done"
elif [ ${build_op} = "clean" ]; then
    echo "cleaning ..."
    clean_build_files
    echo "clean done"
elif [ ${build_op} = "rebuild" ]; then
    echo "rebuilding ..."
    clean_build_files
    cmake_build .
    echo "rebuild done"
fi
