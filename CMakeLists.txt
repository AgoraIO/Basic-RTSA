cmake_minimum_required(VERSION 3.5)
project(AgoraSDKDemo)

#set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_COMPILER clang++)
set(AGORA_MEDIA_SDK_DIR libs/agora_media_sdk)

# Config of AgoraSDKDemoDlApp
FILE(GLOB_RECURSE LOCAL_DEMODLAPP_CPP_FILES "${CMAKE_SOURCE_DIR}/AgoraSDKDemoDlApp/src/*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fsanitize=address")

include_directories(
        /usr/local/include
)

link_libraries(
        pthread
        dl
)

add_executable(AgoraSDKDemoDlApp
        ${LOCAL_DEMODLAPP_CPP_FILES}
)

# Config of AgoraSDKWrapper
FILE(GLOB_RECURSE LOCAL_WRAPPER_CPP_FILES "${CMAKE_SOURCE_DIR}/AgoraSDKWrapper/src/*.cpp")

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${AGORA_MEDIA_SDK_DIR}/include
        ${AGORA_MEDIA_SDK_DIR}/include/api2
        ${CMAKE_SOURCE_DIR}/AgoraSDKWrapper/include
        ${CMAKE_SOURCE_DIR}/AgoraSDKWrapper/src
        /usr/local/include
        /usr/local/include/opus
)

link_directories(
        /usr/local/lib
        /usr/local/lib64
        /usr/lib/x86_64-linux-gnu
        ${AGORA_MEDIA_SDK_DIR}
)

link_libraries(
        ogg
        opusfile
        opus
        agora_rtc_sdk
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fsanitize=address")

add_library(AgoraSDKWrapper
        SHARED
        ${LOCAL_WRAPPER_CPP_FILES}
)

# Config of AgoraSDKDemoApp
FILE(GLOB_RECURSE LOCAL_DEMOAPP_CPP_FILES "${CMAKE_SOURCE_DIR}/AgoraSDKDemoApp/src/*.cpp")

include_directories(
        ${CMAKE_SOURCE_DIR}/AgoraSDKWrapper/include
        ${CMAKE_SOURCE_DIR}/AgoraSDKWrapper/src
        /usr/local/include
)

link_libraries(
        AgoraSDKWrapper
)

add_executable(AgoraSDKDemoApp
        ${LOCAL_DEMOAPP_CPP_FILES}
)

# Config of crash app
FILE(GLOB_RECURSE LOCAL_CRASHAPP_CPP_FILES "${CMAKE_SOURCE_DIR}/Crash/src/*.cpp")

link_libraries(
        agora_rtc_sdk
)

add_executable(CrashApp
        ${LOCAL_CRASHAPP_CPP_FILES}
)
