cmake_minimum_required(VERSION 2.8.2)
project(opusfile-download NONE)

include(ExternalProject)
ExternalProject_Add(opusfile
        GIT_REPOSITORY https://github.i.agoralab.co/believeszw/opusfile.git
        GIT_TAG master
        SOURCE_DIR "${CMAKE_BINARY_DIR}/../third_party/Opusfile/Opusfile-src"
        BINARY_DIR "${CMAKE_BINARY_DIR}/Opusfile-build"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        TEST_COMMAND ""
        )