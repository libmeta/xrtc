project(3rd)

include(GetSrt)
add_subdirectory(jmi)
add_subdirectory(spdlog)
add_subdirectory(mpegts)
add_subdirectory(sigslot)

add_library(3rd::spdlog ALIAS spdlog)
add_library(3rd::sigslot ALIAS sigslot)

target_include_directories(xrtc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(xrtc PRIVATE
    3rd::jmi
    3rd::spdlog
    3rd::sigslot
    3rd::mpegts
    3rd::srt
)
