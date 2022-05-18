project(mpegts)

add_library(MpegtsLib STATIC)
add_library(3rd::mpegts ALIAS MpegtsLib)

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} _SOURCES)
file(GLOB_RECURSE HPP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE CPP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
target_sources(MpegtsLib PRIVATE
    ${HPP_SOURCES}
    ${CPP_SOURCES}
)
