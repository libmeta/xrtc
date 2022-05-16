cmake_minimum_required(VERSION 3.14)

project(xrtc LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(xrtc STATIC)
target_compile_options(xrtc PRIVATE -std=c++17)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(InitSystemName)
add_subdirectory(base)
add_subdirectory(3rd)
add_subdirectory(logger)
add_subdirectory(webrtc)
add_subdirectory(srt_net)
add_subdirectory(app)

target_sources(xrtc PRIVATE
    xrtc.cpp
    xrtc.hpp
)

target_compile_definitions(xrtc PRIVATE XRTC_LIBRARY)

