#include "xrtc.hpp"
#include "mpegts/mpegts_muxer.hpp"

XRTC::XRTC()
{
    auto test = MpegTsMuxer(std::map<uint8_t, int>(),1,2,MpegTsMuxer::MuxType::h222Type);
}
