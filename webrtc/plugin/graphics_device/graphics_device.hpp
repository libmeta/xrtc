#pragma once

#include "igraphics_device.hpp"

namespace xrtc {
namespace webrtc {

    // Singleton
    class GraphicsDevice {
    public:
        static GraphicsDevice& GetInstance();
        IGraphicsDevice* Init(IUnityInterfaces* unityInterface);
        IGraphicsDevice* Init(
            UnityGfxRenderer renderer, void* device, IUnityInterface* unityInterface);

    private:
        GraphicsDevice();
        GraphicsDevice(GraphicsDevice const&) = delete;
        void operator=(GraphicsDevice const&) = delete;
    };

} // end namespace webrtc
} // end namespace xrtc
