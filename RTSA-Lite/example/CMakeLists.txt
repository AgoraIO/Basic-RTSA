cmake_minimum_required(VERSION 3.0)
project(example)

# Build Types
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}
    CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel tsan asan lsan msan ubsan"
    FORCE)

# AddressSanitize
set(CMAKE_C_FLAGS_ASAN
    "-fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O1"
    CACHE STRING "Flags used by the C compiler during AddressSanitizer builds."
          FORCE)
set(CMAKE_CXX_FLAGS_ASAN
    "-fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O1"
    CACHE STRING
          "Flags used by the C++ compiler during AddressSanitizer builds."
          FORCE)

set(CMAKE_C_FLAGS "-std=gnu99")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

add_definitions(-DCONFIG_POSIX)

if(${SDK_LICENSE_ENABLE} STREQUAL "on")
    message("sdk license is enabled")
    add_definitions(-DSDK_LICENSE_ENABLED)
endif()

include_directories(${CMAKE_SOURCE_DIR}/../agora_sdk/include)
link_directories(${CMAKE_SOURCE_DIR}/../agora_sdk/lib)

include_directories(${CMAKE_SOURCE_DIR}/../third-party/media_parser/include)
link_directories(${CMAKE_SOURCE_DIR}/../third-party/media_parser/lib)

add_executable(hello_rtsa hello_rtsa.c)
target_link_libraries(hello_rtsa agora-rtc-sdk media_parser)


include_directories(${CMAKE_SOURCE_DIR}/../third-party/http_parser/include)
include_directories(${CMAKE_SOURCE_DIR}/../third-party/json_parser/include)
link_directories(${CMAKE_SOURCE_DIR}/../third-party/http_parser/lib)

add_executable(license_activator license_activator.c)
target_link_libraries(license_activator agora-rtc-sdk http_parser)


