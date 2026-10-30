#pragma once
#include "vulkan_common.h"

namespace VkRenderer { class App; }
namespace VkRenderer::Geometry { struct Mesh; struct Texture; }

namespace VkRenderer::Utilities
{
    struct MemoryState
    {
        vk::AccessFlags2 accessMask = {};
        vk::PipelineStageFlags2 stageMask = {};
    };
    
    struct ImageState
    {
        vk::ImageLayout layout = vk::ImageLayout::eUndefined;
        u32 queueFamilyIndex = vk::QueueFamilyIgnored;
        vk::AccessFlags2 accessMask = {};
        vk::PipelineStageFlags2 stageMask = {};
    };

    class CommandBufferHelper
    {
    private:
        App& app_;
        vk::raii::CommandBuffer& cmdBuffer_;
    
    public:
        CommandBufferHelper() = delete;
        CommandBufferHelper(App& app, vk::raii::CommandBuffer& cmdBuffer) : app_(app), cmdBuffer_(cmdBuffer) {}

        constexpr vk::raii::CommandBuffer& operator*() { return this->cmdBuffer_; }
        constexpr vk::raii::CommandBuffer* operator->() { return &this->cmdBuffer_; }

        void bindMesh(const Geometry::Mesh& mesh);
        void drawMesh(const Geometry::Mesh& mesh, u32 instanceCount = 1);
        
        MemoryState addMemoryBarrier(MemoryState srcState, MemoryState dstState);
        ImageState addImageBarrier(const Geometry::Texture& tex, ImageState srcState,
            ImageState dstState, Option<vk::ImageSubresourceRange> subresourceRangeOverride = {});
        
        template <typename T>
        void pushConstants(T& constants, vk::PipelineLayout pipelineLayout, vk::ShaderStageFlags stages)
        {
            this->cmdBuffer_.pushConstants(pipelineLayout, stages, 0, sizeof(T), &constants);
        }
    };
}