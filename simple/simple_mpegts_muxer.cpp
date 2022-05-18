#include <cassert>
#include <map>

#include "simple_mpegts_muxer.hpp"

namespace xrtc {

SimpleMpegtsMuxer::SimpleMpegtsMuxer(const std::vector<StreamType>& stream_types)
{
    std::map<uint8_t, int> stream_map;
    std::for_each(stream_types.begin(), stream_types.end(), [&stream_map](const StreamType& stream_type) {
        if (getMediaType(stream_type) == MediaType::_VIDEO) {
            stream_map[(uint8_t)stream_type] = VIDEO_PID;
        } else if (getMediaType(stream_type) == MediaType::_AUDIO) {
            stream_map[(uint8_t)stream_type] = AUDIO_PID;
        }
    });

    assert(!stream_map.empty());

    ts_muxer_ = std::make_shared<decltype(ts_muxer_)::element_type>(stream_map, PMT_PID, PCR_PID, MUX_TYPE);
    ts_muxer_->tsOutCallback = [this](SimpleBuffer& rSb, uint8_t lTag, bool lRandomAccess) {
        if (output_) {
            output_(rSb.data(), rSb.size(), lRandomAccess);
        }
    };
}

SimpleMpegtsMuxer::~SimpleMpegtsMuxer()
{
}

void SimpleMpegtsMuxer::setOutputCallBack(const OutputCallback& output)
{
    std::lock_guard<decltype(mutex_)> lk(mutex_);
    output_ = output;
}

void SimpleMpegtsMuxer::encode(const uint8_t* data, int data_size, int64_t dts_us, const StreamType& stream_type, bool is_key_frame)
{
    std::lock_guard<decltype(mutex_)> lk(mutex_);
    EsFrame es_frame;
    es_frame.mData = std::make_shared<SimpleBuffer>();
    es_frame.mData->append(data, data_size);
    es_frame.mPts = dts_us;
    es_frame.mDts = dts_us;
    es_frame.mPcr = 0;
    es_frame.mStreamType = (uint8_t)stream_type;
    es_frame.mStreamId = (uint16_t)getMediaType(stream_type);
    es_frame.mPid = VIDEO_PID;
    es_frame.mExpectedPesPacketLength = 0;
    es_frame.mRandomAccess = is_key_frame ? 1 : 0;
    es_frame.mCompleted = true;

    ts_muxer_->encode(es_frame);
}

SimpleMpegtsMuxer::MediaType SimpleMpegtsMuxer::getMediaType(const StreamType& stream_type)
{
    if (stream_type == StreamType::_AVC || stream_type == StreamType::_HEVC) {
        return MediaType::_VIDEO;
    } else if (stream_type == StreamType::_AAC || stream_type == StreamType::_OPUS) {
        return MediaType::_AUDIO;
    }

    return MediaType::_UNKNOWN;
}

}
