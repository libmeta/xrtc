#include "video_track_source.hpp"

namespace xrtc {

namespace webrtc {

    rtc::scoped_refptr<VideoTrackSource> VideoTrackSource::Create(bool is_screencast, absl::optional<bool> needs_denoising)
    {
        return new rtc::RefCountedObject<VideoTrackSource>(is_screencast, needs_denoising);
    }

    VideoTrackSource::VideoTrackSource(
        bool is_screencast,
        absl::optional<bool> needs_denoising)
        : AdaptedVideoTrackSource(1)
        , is_screencast_(is_screencast)
        , needs_denoising_(needs_denoising)
    {
    }

    VideoTrackSource::~VideoTrackSource()
    {
        {
            std::unique_lock<std::shared_timed_mutex> lock(m_mutex);
        }
    }

    MediaSourceInterface::SourceState VideoTrackSource::state() const
    {
        return MediaSourceInterface::SourceState::kLive;
    }

    bool VideoTrackSource::remote() const
    {
        return false;
    }

    bool VideoTrackSource::is_screencast() const
    {
        return is_screencast_;
    }

    absl::optional<bool> VideoTrackSource::needs_denoising() const
    {
        return needs_denoising_;
    }

    void VideoTrackSource::SendFeedback()
    {
        // float maxFramerate = video_adapter()->GetMaxFramerate();
        //  todo(kazuki):
    }

    void VideoTrackSource::OnFrameCaptured(rtc::scoped_refptr<VideoFrame> frame)
    {
        const std::unique_lock<std::shared_timed_mutex> lock(m_mutex, std::try_to_lock);
        if (!lock) {
            // currently encoding
            return;
        }

        SendFeedback();

//        const int64_t now_us = rtc::TimeMicros();
//        const int64_t translated_camera_time_us = timestamp_aligner_.TranslateTimestamp(frame->timestamp().us(), now_us);

//        rtc::scoped_refptr<VideoFrameAdapter> frame_adapter(
//            new rtc::RefCountedObject<VideoFrameAdapter>(std::move(frame)));

//        ::webrtc::VideoFrame::Builder builder = ::webrtc::VideoFrame::Builder()
//                                                    .set_video_frame_buffer(frame_adapter)
//                                                    .set_timestamp_us(translated_camera_time_us);
//        OnFrame(builder.build());
    }

}
}
