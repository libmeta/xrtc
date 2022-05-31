#include "igraphics_device.hpp"
#include "pch.hpp"

namespace xrtc {
namespace webrtc {

    IGraphicsDevice::IGraphicsDevice(UnityGfxRenderer renderer)
        : m_gfxRenderer(renderer)
    {
    }

    IGraphicsDevice::~IGraphicsDevice()
    {
    }

} // end namespace webrtc
} // end namespace xrtc
