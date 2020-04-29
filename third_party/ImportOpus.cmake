cmake_minimum_required(VERSION 2.8.2)
project(opus-download NONE)

include(ExternalProject)
ExternalProject_Add(opus
        GIT_REPOSITORY https://github.i.agoralab.co/believeszw/opus.git
        GIT_TAG master
        SOURCE_DIR "${CMAKE_BINARY_DIR}/../third_party/Opus/Opus-src"
        BINARY_DIR "${CMAKE_BINARY_DIR}/Opus-build"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        TEST_COMMAND ""
        )