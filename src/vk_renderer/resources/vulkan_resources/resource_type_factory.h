#pragma once
#include "../../utilities/vulkan_common.h"

namespace VkRenderer::Resources
{
    struct VulkanResourceContext
    {
        const vk::raii::Instance& instance;
        const vk::raii::Device& device;
        const vk::raii::PhysicalDevice& physicalDevice;
    };

    class VulkanResourceTypeFactory
    {
    private:
        const VulkanResourceContext* context_ = nullptr;

    public:
        virtual ~VulkanResourceTypeFactory() = default;

        void setContext(const VulkanResourceContext& context)
        {
            this->context_ = &context;            
        }

        const VulkanResourceContext& getContext() const
        {
            assert(this->context_ != nullptr);
            return *this->context_;
        }
    };
}