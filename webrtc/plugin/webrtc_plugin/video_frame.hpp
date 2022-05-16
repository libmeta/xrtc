#pragma once

#include "api/units/time_delta.h"
#include "rtc_base/ref_counted_object.h"
#include "rtc_base/timestamp_aligner.h"

namespace xrtc {

namespace webrtc {

    using namespace ::webrtc;

    class VideoFrame : public rtc::RefCountInterface {
    public:
        VideoFrame();
    };


}

}
