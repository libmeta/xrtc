project(graphics_device)

target_sources(WebRTCLib PRIVATE
    graphics_device.cpp
    graphics_device.hpp
    igraphics_device.cpp
    igraphics_device.hpp
    itexture_2d.cpp
    itexture_2d.hpp
)

#if(Windows)
#    add_subdirectory(Vulkan)
#    add_subdirectory(D3D11)
#    add_subdirectory(D3D12)
#    add_subdirectory(Cuda)
#elseif(iOS OR macOS)
#    add_subdirectory(Metal)
#elseif(Linux)
#    add_subdirectory(Vulkan)
#    add_subdirectory(OpenGL)
#    add_subdirectory(Cuda)
#elseif(Android)
#    add_subdirectory(Vulkan)
#    add_subdirectory(OpenGL)
#endif()

