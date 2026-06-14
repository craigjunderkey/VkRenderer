#include "../hello_triangle_application.h"
#include "depth_resources.h"

namespace HelloTriangleApplication::Geometry
{
    vk::Format findSupportedFormat(App& app, const std::vector<vk::Format>& candidates,
        vk::ImageTiling tiling, vk::FormatFeatureFlags features)
    {
        return vk::Format::eD32Sfloat;
    }
    
    Result<Unit, std::string> createDepthTexture(App& app)
    {
        return Ok();
    }
}