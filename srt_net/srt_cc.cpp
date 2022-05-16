#include "srt_cc.hpp"

namespace xrtc {

SrtCongestionCtrl::SrtCongestionCtrl(SRTSOCKET sock)
    : sock(sock)
{
}

SrtCongestionCtrl::~SrtCongestionCtrl()
{
}

bool SrtCongestionCtrl::updateVideoEncodeBitate(int64_t& current_vencode_bitrate, const int64_t video_stream_bitrate)
{
    static constexpr double INCR_RATION = 1.08;
    static constexpr double DECR_RATION = 0.85;

    int64_t tmp_vencode_bitrate = current_vencode_bitrate;
    if (update_vencode_bitrate_task_.doByTimeAndCount([this, &current_vencode_bitrate, &video_stream_bitrate, &tmp_vencode_bitrate] {
            if (congestion_state_ == STATE_INCR) { // not adjust bitrate for bandwith is enough
                if (current_vencode_bitrate < video_stream_bitrate * 1.3) {
                    tmp_vencode_bitrate = current_vencode_bitrate * INCR_RATION;
                    xlogt("increase");
                }
            } else if (congestion_state_ == STATE_DECR) { // adjust bitrate to estimate_bandwith cbr
                if (current_vencode_bitrate > video_stream_bitrate * 0.4) {
                    tmp_vencode_bitrate = current_vencode_bitrate * DECR_RATION;
                    xlogt("decrease");
                }
            }
        })) {
        update_vencode_bitrate_task_.reset();
        if (tmp_vencode_bitrate != current_vencode_bitrate) {
            current_vencode_bitrate = tmp_vencode_bitrate;
            return true;
        } else {
            xlogt("keep");
        }
    }

    return false;
}

void SrtCongestionCtrl::congestionCtrl()
{
    if (srt_congestion_ctrl_task_.doByTimeAndCount([this] {
            SRT_TRACEBSTATS perf;
            const auto srt_bstats_result = srt_bstats(sock, &perf, 1);
            if (srt_bstats_result != 0) {
                return;
            }

            const int64_t inflight = perf.pktFlightSize * 188 * 7;
            const int64_t bw_bitrate = perf.mbpsSendRate * 1000 * 1000; //_last_send_bytes*8*1000/diff_t;
            const int rtt = std::max((int)(perf.msRTT), RTT_MIN);

            updateRTT(rtt);
            updateMaxBW(bw_bitrate);
            congestion_state_ = srtBitrateGetState(inflight);

            xlogt("congestion ctrl({}) return {}, rtt:{}, inflight:{}, bw_bitrate:{}",
                sock, srt_bstats_result, rtt, inflight, bw_bitrate);
        })) {
        srt_congestion_ctrl_task_.reset();
    }
}

SRT_SOCKSTATUS SrtCongestionCtrl::getSockstate() const
{
    return srt_getsockstate(sock);
}

bool SrtCongestionCtrl::getSndBuffer(size_t* bytes, size_t* blocks)
{
    const int st = srt_getsndbuffer(sock, blocks, bytes);
    if (st == SRT_ERROR) {
        return false;
    }

    return true;
}

void SrtCongestionCtrl::updateRTT(int rtt)
{
    rtt_array_.push_back(rtt = std::max(rtt, RTT_MIN));
    if ((rtt_ready_ = rtt_array_.size() >= RTT_LIST_MAX)) {
        rtt_min_ = *std::min_element(rtt_array_.begin(), rtt_array_.end());
        rtt_array_.clear();
        xlogt("update rtt={}, input rtt:{}", rtt_min_, rtt);
    }
}

void SrtCongestionCtrl::updateMaxBW(uint64_t maxbw)
{
    bw_array_.push_back(maxbw);
    if ((bw_ready_ = bw_array_.size() >= BW_LIST_MAX)) {
        avg_bw_ = std::accumulate(bw_array_.begin(), bw_array_.end(), 0) / bw_array_.size();
        bw_max_ = *std::max_element(bw_array_.begin(), bw_array_.end());
        bw_max_ = (bw_max_ + avg_bw_) * 1.2 / 2;
        bw_array_.clear();
    }
}

int SrtCongestionCtrl::srtBitrateGetState(int inflight)
{
    static constexpr int64_t INCR = 1316 * 8 * 3;
    static constexpr int64_t DECR = -1316 * 8 * 6;

    if (!rtt_ready_ || !bw_ready_) {
        return STATE_KEEP;
    }

    const double bdp = bw_max_ * rtt_min_ / 1000;
    inflight = inflight * 8;
    const int64_t current_state = cwnd_gain_ * bdp - inflight;
    state_array_.push_back(current_state);
    if (!(state_ready_ = state_array_.size() >= STATE_LIST_MAX)) {
        return STATE_KEEP;
    }

    const int64_t final_state = std::accumulate(state_array_.begin(), state_array_.end(), 0l);
    xlogt("get congestion inflight:{}, _bw_max:{}, _bw_avg:{}, _rtt_min:{}, bdp:{}, final_state:{}, current_state:{}",
        inflight, bw_max_, avg_bw_, rtt_min_, bdp, final_state, current_state);

    if (final_state > (INCR)) {
        return STATE_INCR;
    } else if (final_state < (DECR - 1)) {
        return STATE_DECR;
    }

    return STATE_KEEP;
}

}
