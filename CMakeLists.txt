cmake_minimum_required(VERSION 3.5)
project(AgoraSDKDemo)

# ogg begin
# Download and unpack googletest at configure time
configure_file(ImportOgg.cmake ogg-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/ogg-download"
        )
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/ogg-download"
        )

# Add ogg directly to our build. This adds the following targets:
add_subdirectory("${CMAKE_BINARY_DIR}/ogg-src"
        "${CMAKE_BINARY_DIR}/ogg-build"
        )

# The ogg targets carry header search path dependencies
# automatically when using CMake 2.8.11 or later. Otherwise we
# have to add them here ourselves.
include_directories("${libogg_SOURCE_DIR}/include")
include_directories("${libogg_SOURCE_DIR}/../ogg-build/include")
# ogg end

# opus begin
# Download and unpack opus at configure time
configure_file(ImportOpus.cmake opus-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/opus-download"
        )
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/opus-download"
        )

# Add opus directly to our build. This adds the following targets:
add_subdirectory("${CMAKE_BINARY_DIR}/opus-src"
        "${CMAKE_BINARY_DIR}/opus-build"
        )

# The opus targets carry header search path dependencies
# automatically when using CMake 2.8.11 or later. Otherwise we
# have to add them here ourselves.
include_directories("${Opus_SOURCE_DIR}/include")
# opus end

# opusfile begin
# Download and unpack opusfile at configure time
configure_file(ImportOpusfile.cmake opusfile-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/opusfile-download"
        )
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/opusfile-download"
        )

# Add opusfile directly to our build. This adds the following targets:
add_subdirectory("${CMAKE_BINARY_DIR}/opusfile-src"
        "${CMAKE_BINARY_DIR}/opusfile-build"
        )

# The opusfile targets carry header search path dependencies
# automatically when using CMake 2.8.11 or later. Otherwise we
# have to add them here ourselves.
include_directories("${Opusfile_SOURCE_DIR}/include")
# opusfile end

# GoogleTest begin
# Download and unpack googletest at configure time
configure_file(ImportGtest.cmake googletest-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
)
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This adds the following targets:
# gtest, gtest_main, gmock and gmock_main
add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
        "${CMAKE_BINARY_DIR}/googletest-build"
)

# The gtest/gmock targets carry header search path dependencies
# automatically when using CMake 2.8.11 or later. Otherwise we
# have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif ()
# GoogleTest end

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_COMPILER g++)
set(AGORA_MEDIA_SDK_DIR libs/agora_media_sdk)

# Config of AgoraSDKDemoDlApp
FILE(GLOB_RECURSE LOCAL_DEMODLAPP_CPP_FILES "${CMAKE_SOURCE_DIR}/src/dynamic_load_demo_main/*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address -Werror -std=c++11")

include_directories(
        /usr/local/include
)

link_libraries(
        dl
        pthread
)

add_executable(AgoraSDKDemoDlApp
        ${LOCAL_DEMODLAPP_CPP_FILES}
)

# Config of AgoraSDKWrapper
FILE(GLOB_RECURSE LOCAL_WRAPPER_CPP_FILES "${CMAKE_SOURCE_DIR}/src/rtc/*"
        "${CMAKE_SOURCE_DIR}/src/utils/*"
        "${CMAKE_SOURCE_DIR}/src/wrapper/*"
)

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${AGORA_MEDIA_SDK_DIR}/include
        ${AGORA_MEDIA_SDK_DIR}/include/api2
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/test_data
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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address -std=c++11")

add_library(AgoraSDKWrapper
        SHARED
        ${LOCAL_WRAPPER_CPP_FILES}
)

# Config of AgoraSdkDemoApp
FILE(GLOB_RECURSE LOCAL_DEMOAPP_CPP_FILES "${CMAKE_SOURCE_DIR}/src/demo_main/main.cpp"
        "${CMAKE_SOURCE_DIR}/src/wrapper_wrapper/*"
)

include_directories(
        ${CMAKE_SOURCE_DIR}/src/include
        /usr/local/include
)

link_libraries(
        AgoraSDKWrapper
)

add_executable(AgoraSdkDemoApp
        ${LOCAL_DEMOAPP_CPP_FILES}
)

# Config of AgoraServerSdkDemoApp
FILE(GLOB_RECURSE LOCAL_SERVER_DEMOAPP_CPP_FILES "${CMAKE_SOURCE_DIR}/src/demo_main/server_sdk_demo.cpp"
        "${CMAKE_SOURCE_DIR}/src/wrapper_wrapper/*"
)

include_directories(
        ${CMAKE_SOURCE_DIR}/src/include
        /usr/local/include
)

link_libraries(
        AgoraSDKWrapper
)
add_executable(AgoraServerSdkDemoApp
        ${LOCAL_SERVER_DEMOAPP_CPP_FILES}
)

SET(EXECUTABLE_OUTPUT_PATH bin/${COMPILE_PLAT})
SET(LIBRARY_OUTPUT_PATH  bin/${COMPILE_PLAT})
