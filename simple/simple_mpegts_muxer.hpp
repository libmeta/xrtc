#pragma once

#include <functional>
#include <string>

#include "mpegts/mpegts_muxer.hpp"
#include "mpegts/ts_pid.hpp"

namespace xrtc {

class SimpleMpegtsMuxer {
    static const int VIDEO_PID = 256;
    static const int AUDIO_PID = 257;
    static const int PMT_PID = 100;
    static const int PCR_PID = VIDEO_PID;
    static const MpegTsMuxer::MuxType MUX_TYPE = MpegTsMuxer::MuxType::h222Type;

public:
    using OutputCallback = std::function<void(const uint8_t*, int, bool)>;

    enum class MediaType : uint16_t {
        _UNKNOWN,
        _VIDEO = (uint16_t)TsPID::PES_VIDEO_ID,
        _AUDIO = (uint16_t)TsPID::PES_AUDIO_ID,
    };

    enum class StreamType : uint8_t {
        _AVC = (uint8_t)TsPID::STREAM_TYPE_VIDEO_H264,
        _HEVC = (uint8_t)TsPID::STREAM_TYPE_VIDEO_HEVC,
        _AAC = (uint8_t)TsPID::STREAM_TYPE_AUDIO_AAC,
        _OPUS = (uint8_t)TsPID::STREAM_TYPE_AUDIO_OPUS,
    };

public:
    explicit SimpleMpegtsMuxer(const std::vector<StreamType>& stream_types);

    ~SimpleMpegtsMuxer();

    void setOutputCallBack(const OutputCallback& output);

    void encode(const uint8_t* data, int data_size, int64_t dts_us, const StreamType& stream_type, bool is_key_frame = true);

private:
    static MediaType getMediaType(const StreamType& stream_type);

private:
    std::mutex mutex_;
    std::shared_ptr<MpegTsMuxer> ts_muxer_ = nullptr;
    OutputCallback output_ = nullptr;
};

}
