cmake_minimum_required(VERSION 2.8.2)
project(ogg-download NONE)

include(ExternalProject)
ExternalProject_Add(ogg
        GIT_REPOSITORY https://github.i.agoralab.co/believeszw/ogg.git
        GIT_TAG master
        #        URL https://downloads.xiph.org/releases/ogg/libogg-1.3.3.tar.xz
        SOURCE_DIR "${CMAKE_BINARY_DIR}/ogg-src"
        BINARY_DIR "${CMAKE_BINARY_DIR}/ogg-build"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        TEST_COMMAND ""
        )