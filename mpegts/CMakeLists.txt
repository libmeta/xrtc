project(mpegts)

add_library(MpegtsLib STATIC)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} _SOURCES)
target_sources(MpegtsLib PRIVATE
    ${_SOURCES}
)
