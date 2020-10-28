cmake_minimum_required(VERSION 3.5)
project(AgoraSDKDemo)

# Remove c++11 to avoid third_party libs build error
separate_arguments(CMAKE_CXX_FLAGS)
list(REMOVE_ITEM CMAKE_CXX_FLAGS "-std=c++11")

foreach(item ${CMAKE_CXX_FLAGS})
    set(TEMP "${TEMP} ${item}")
endforeach(item)
set(CMAKE_CXX_FLAGS ${TEMP})

if("${MEM_CHECK}" STREQUAL "true")
        set(sanitizer -fsanitize=address)
endif()

if("${TARGET}" STREQUAL "rtc")
        set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -DRTC_TARGET ")
endif()

if("${TARGET_OS}" STREQUAL "linux")
        set(lib_sdk "agora_rtc_sdk")
        set(lib_thread "pthread")
        if("${TARGET}" STREQUAL "rtc")
                set(rtc_src "${CMAKE_SOURCE_DIR}/src/rtc/*")
        endif()
        set(AGORA_MEDIA_SDK_DIR ${CMAKE_SOURCE_DIR}/libs/agora_media_sdk)
        set(AGORA_MEDIA_SDK_HEADER_DIR ${CMAKE_SOURCE_DIR}/libs/agora_media_sdk)
        set(COMPILE_PLAT "x64")
else()
        if("${TARGET}" STREQUAL "rtsa")
                set(lib_sdk "agora-rtsa-sdk")
        else()
                set(lib_sdk "agora-rtc-sdk-jni")
        endif()
        set(lib_thread "stdc++")
        set(AGORA_MEDIA_SDK_DIR ${CMAKE_SOURCE_DIR}/../agora_rtsa_sdk/libs/${ANDROID_ABI})
        set(AGORA_MEDIA_SDK_HEADER_DIR ${CMAKE_SOURCE_DIR}/../agora_rtsa_sdk/)
        set(COMPILE_PLAT "${ANDROID_ABI}")
endif()


###    1. Prepare third-party libs    ###
set(third_party_path "${CMAKE_BINARY_DIR}/../third_party/")

macro(PREPARE_LIBS libA)
configure_file(${third_party_path}/Import${ARGV0}.cmake
        ${third_party_path}/${ARGV0}/${ARGV0}-download/CMakeLists.txt
)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${third_party_path}/${ARGV0}/${ARGV0}-download"
)
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${third_party_path}/${ARGV0}/${ARGV0}-download"
)
add_subdirectory("${third_party_path}/${ARGV0}/${ARGV0}-src"
        "${CMAKE_BINARY_DIR}/${ARGV0}-build"
)
endmacro()

# include header path
include_directories("${third_party_path}/Ogg/Ogg-src/include")
include_directories("${CMAKE_BINARY_DIR}/Ogg-build/include")
include_directories("${third_party_path}/Opus/Opus-src/include")
include_directories("${third_party_path}/Opusfile/Opusfile-src/include")
include_directories("${third_party_path}/Gtest/Gtest-src/googletest/include")

# download libs and declare subdirectory
PREPARE_LIBS(Ogg)

# Opus depends ogg
link_libraries(
        ogg
)
PREPARE_LIBS(Opus)

# opusfile depends ogg and opus
link_libraries(
        opus
)
PREPARE_LIBS(Opusfile)

PREPARE_LIBS(Gtest)


###    2. Build common libs    ###
set(CMAKE_DEBUG_POSTFIX "")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer ${sanitizer} -Werror -std=c++14")

# Config of AgoraSDKWrapper
FILE(GLOB_RECURSE LOCAL_WRAPPER_CPP_FILES ${rtc_src}
        "${CMAKE_SOURCE_DIR}/src/utils/*"
        "${CMAKE_SOURCE_DIR}/src/wrapper/*"
)

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/include
        ${CMAKE_SOURCE_DIR}/test_data
        ${AGORA_MEDIA_SDK_HEADER_DIR}/include
        ${AGORA_MEDIA_SDK_HEADER_DIR}/include/base
        ${AGORA_MEDIA_SDK_HEADER_DIR}/include/api2
)

link_directories(
        ${AGORA_MEDIA_SDK_DIR}
)

link_libraries(
        opusfile
        ${lib_thread}
        ${lib_sdk}
)

add_library(AgoraSDKWrapper
        SHARED
        ${LOCAL_WRAPPER_CPP_FILES}
)


###    3. Build AgoraSDKQuickstart    ###
# Config of AgoraSDKQuickstart
FILE(GLOB_RECURSE LOCAL_DEMOAPP_CPP_FILES "${CMAKE_SOURCE_DIR}/src/AgoraSDKQuickstart/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/wrapper_wrapper/*"
)

link_libraries(
        AgoraSDKWrapper
)

add_executable(AgoraSDKQuickstart
        ${LOCAL_DEMOAPP_CPP_FILES}
)


###    4. Build AgoraSDKQuickstart    ###
link_libraries(
        dl
)

# Config of AgoraSDKQuickstartDl
FILE(GLOB_RECURSE LOCAL_DEMODLAPP_CPP_FILES "${CMAKE_SOURCE_DIR}/src/AgoraSDKQuickstartDl/*.cpp")

add_executable(AgoraSDKQuickstartDl
        ${LOCAL_DEMODLAPP_CPP_FILES}
)


###    5. Build AgoraServerSdkDemoApp    ###

# Config of AgoraServerSdkDemoApp
FILE(GLOB_RECURSE LOCAL_SERVER_DEMOAPP_CPP_FILES "${CMAKE_SOURCE_DIR}/src/AgoraServerSdkDemoApp/*.cpp")

add_executable(AgoraServerSdkDemoApp
        ${LOCAL_SERVER_DEMOAPP_CPP_FILES}
)


###    6. Build AgoraSamplesDemoApp    ###
# Config for gtest demo build
add_subdirectory(src/samples)

SET(EXECUTABLE_OUTPUT_PATH bin/${COMPILE_PLAT})
SET(LIBRARY_OUTPUT_PATH  bin/${COMPILE_PLAT})
