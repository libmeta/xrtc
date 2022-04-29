#include "xrtc.hpp"
#include "mpegts/mpegts_muxer.hpp"
#include "logger/xlog.hpp"
#include "srt.h"

XRTC::XRTC()
{
    MpegTsMuxer(std::map<uint8_t, int>(),1,2,MpegTsMuxer::MuxType::h222Type);
    srt_create_socket();
}
