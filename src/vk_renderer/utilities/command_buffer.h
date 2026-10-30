#pragma once
#include "common.h"
#include "queue.h"
#include "vulkan_common.h"

namespace VkRenderer::Utilities
{
    Result<Unit, ErrorMessage> createCommandBuffers(App& app);
    
    /// @brief Begins a compute comand buffer, dispatches, then submits to the graphics queue.
    /// @post pipeline is particle-compute
    Result<Unit, ErrorMessage> dispatchCompute(App& app);
    
    void beginRender(App& app, u32 imageIndex);
    void endRender(App& app, u32 imageIndex);

    Result<vk::raii::CommandBuffer, ErrorMessage> beginSingleTimeCommands(App& app, FatQueue& queue);
    void endSingleTimeCommands(vk::raii::CommandBuffer&& commandBuffer, FatQueue& queue);
}