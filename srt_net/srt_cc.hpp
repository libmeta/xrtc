#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <numeric>
#include <vector>

#include <srt.h>

#include "base/run_task.hpp"
#include "logger/logger.hpp"

namespace xrtc {

class SrtCongestionCtrl {
    enum {
        STATE_INCR = 0x01,
        STATE_DECR = 0x02,
        STATE_KEEP = 0x03,
    };

    static constexpr auto VIDEO_UPDATE_INTERVAL = std::chrono::milliseconds(500);
    static constexpr auto SRT_CHECK_INTERVAL = std::chrono::milliseconds(300);

    static constexpr int RTT_LIST_MAX = 6;
    static constexpr int BW_LIST_MAX = 6;
    static constexpr int STATE_LIST_MAX = 6;
    static constexpr double CWND_GAIN_DEF = 1.3;
    static constexpr int RTT_INIT = 100;
    static constexpr int RTT_MIN = 35;

public:
    explicit SrtCongestionCtrl(SRTSOCKET sock);

    ~SrtCongestionCtrl();

    bool updateVideoEncodeBitate(int64_t& current_vencode_bitrate, const int64_t video_stream_bitrate);

    void congestionCtrl();

    SRT_SOCKSTATUS getSockstate() const;

    bool getSndBuffer(size_t* bytes, size_t* blocks);

private:
    void updateRTT(int rtt);

    // update current max bandwith(bits/s)
    void updateMaxBW(uint64_t maxbw);

    // get the state whether adjust the encoder bitrate.
    int srtBitrateGetState(int inflight);

private:
    SRTSOCKET sock;

    std::vector<int> rtt_array_;
    int rtt_min_ = RTT_INIT; // ms
    bool rtt_ready_ = false;

    std::vector<uint64_t> bw_array_; // bits/s
    uint64_t bw_max_ = 0; // bits/s, max srt bandwidth
    uint64_t avg_bw_ = 0; // bits/s, avg srt bandwidth
    bool bw_ready_ = false;

    std::vector<int64_t> state_array_;
    bool state_ready_ = false;

    double cwnd_gain_ = CWND_GAIN_DEF;
    std::atomic<int> congestion_state_ = STATE_KEEP;

    RunTask update_vencode_bitrate_task_ { VIDEO_UPDATE_INTERVAL };
    RunTask srt_congestion_ctrl_task_ { SRT_CHECK_INTERVAL };
};

}
