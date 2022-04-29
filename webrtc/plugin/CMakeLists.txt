cmake_minimum_required(VERSION 3.18)
project(webrtc
    VERSION 2.4.0
    LANGUAGES C CXX ASM
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# enable debug output
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)

find_package(Threads REQUIRED)
find_package(WebRTC REQUIRED)
add_subdirectory(webrtc_plugin)
