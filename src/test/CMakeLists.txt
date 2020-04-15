project(Test)

FILE(GLOB_RECURSE LOCAL_RTC_TEST_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(AGORA_MEDIA_SDK_DIR ${CMAKE_SOURCE_DIR}/libs/agora_media_sdk)

include_directories(${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/include
        ${AGORA_MEDIA_SDK_DIR}/include
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
        asan
        agora_rtc_sdk
        AgoraSDKWrapper
        gtest gtest_main -lpthread
)

add_executable(AgoraGTestApp ${LOCAL_RTC_TEST_CPP_FILES})

SET(EXECUTABLE_OUTPUT_PATH ../../bin/${COMPILE_PLAT})
