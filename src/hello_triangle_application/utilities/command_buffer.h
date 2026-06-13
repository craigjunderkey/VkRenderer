#pragma once
#include "common.h"
#include "queue.h"
#include "vulkan_common.h"

namespace HelloTriangleApplication::Utilities
{
    Result<Unit, std::string> createCommandBuffers(App& app);
    void recordCommandBuffer(App& app, u32 imageIndex);
    Result<vk::raii::CommandBuffer, std::string> beginSingleTimeCommands(App& app, FatQueue& queue);
    void endSingleTimeCommands(vk::raii::CommandBuffer&& commandBuffer, FatQueue& queue);
    Result<Unit, std::string> transitionImageLayout(vk::raii::CommandBuffer &commandBuffer, 
        const vk::Image &image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
        u32 srcFamily = vk::QueueFamilyIgnored, u32 dstFamily = vk::QueueFamilyIgnored);
}