#pragma once
#include "../utilities/common.h"
#include "buffer.h"
#include "../utilities/vulkan_common.h"

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, std::string> createBuffers(App& app);
    Result<Unit, std::string> copyBuffer(App& app, FatBuffer& src, FatBuffer& dst);
    void copyBufferToImage(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer,
        vk::raii::Image& image, u32 width, u32 height);
}