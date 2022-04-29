cmake_minimum_required(VERSION 3.14)

project(xrtc LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(InitSystemName)

add_subdirectory(mpegts)
add_subdirectory(webrtc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(xrtc STATIC
  xrtc.cpp
  xrtc.hpp
)

target_link_libraries(xrtc PRIVATE
    MpegtsLib
)

target_compile_definitions(xrtc PRIVATE XRTC_LIBRARY)

add_subdirectory(app)
