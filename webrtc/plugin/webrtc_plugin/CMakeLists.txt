add_library(WebRTCLib STATIC)

target_sources(WebRTCLib PRIVATE
    pch.cpp
    pch.hpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(WebRTCLib PUBLIC
        -Wall
        -Wextra
        -Wformat=2
        -Wno-unused-parameter )
endif()

target_compile_definitions(WebRTCLib PUBLIC $<$<CONFIG:Debug>:DEBUG>)

if(Android)
    target_compile_definitions(WebRTCLib PUBLIC
        WEBRTC_ANDROID
        WEBRTC_POSIX
        VK_NO_PROTOTYPES )

    target_link_libraries(WebRTCLib PRIVATE
        log
        OpenSLES
        GLESv3
        EGL )
    target_compile_options(WebRTCLib PUBLIC -fno-lto -fno-rtti )

endif()

target_include_directories(WebRTCLib SYSTEM PUBLIC ${WEBRTC_INCLUDE_DIR} )

target_include_directories(WebRTCLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

