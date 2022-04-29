project(3rd)

include(GetSrt)
add_subdirectory(spdlog)
add_subdirectory(mpegts)
add_subdirectory(sigslot)

target_include_directories(xrtc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIB_MPEGTS_INCLUDE}
    ${LIB_SRT_INCLUDE}
)

target_link_libraries(xrtc PRIVATE
    srt::srt_static_lib
    spdlog::spdlog
    mpegts::MpegtsLib
    Pal::Sigslot
)
