#pragma once

#include <shared_mutex>

#include "pch.hpp"

namespace xrtc {

namespace webrtc {
    using namespace ::webrtc;

    class VideoTrackSource : public rtc::AdaptedVideoTrackSource {
    public:
        VideoTrackSource(bool is_screencast, absl::optional<bool> needs_denoising);

        ~VideoTrackSource() override;

        SourceState state() const override;

        bool remote() const override;
        bool is_screencast() const override;
        absl::optional<bool> needs_denoising() const override;
        void OnFrameCaptured(rtc::scoped_refptr<VideoFrame> frame);

        using ::webrtc::VideoTrackSourceInterface::AddOrUpdateSink;
        using ::webrtc::VideoTrackSourceInterface::RemoveSink;

        static rtc::scoped_refptr<VideoTrackSource> Create(bool is_screencast, absl::optional<bool> needs_denoising);

    private:
        void SendFeedback();

    private:
        rtc::TimestampAligner timestamp_aligner_;

        const bool is_screencast_;
        const absl::optional<bool> needs_denoising_;
        std::shared_timed_mutex m_mutex;
    };

}

}
