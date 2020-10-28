cmake_minimum_required(VERSION 3.5)
project(AgoraSamplesDemoProject)

if("${TARGET}" STREQUAL "rtc")
        set(test_rtc_src "${CMAKE_CURRENT_SOURCE_DIR}/rtc/*")
endif()

FILE(GLOB_RECURSE LOCAL_RTC_TEST_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/rtsa/*.cpp" ${test_rtc_src})

include_directories(${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/include
        ${AGORA_MEDIA_SDK_DIR}/include
)

link_directories(
        ${CMAKE_BINARY_DIR}/lib
)

link_libraries(
        gtest
        gtest_main
        gmock
        gmock_main
)

add_executable(AgoraSamplesDemoApp ${LOCAL_RTC_TEST_CPP_FILES})

SET(EXECUTABLE_OUTPUT_PATH ../../bin/${COMPILE_PLAT})