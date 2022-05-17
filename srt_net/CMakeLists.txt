project(srt_net)

target_sources( xrtc PRIVATE
    srt_net.cpp
    srt_net.hpp
    srt_cc.cpp
    srt_cc.hpp

    test_srt_net.cpp
    test_srt_net.hpp

)
