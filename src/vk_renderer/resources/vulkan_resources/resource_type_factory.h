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

    /* Overall design of a TFactory:
    - struct TDescription
        {
            ...
            vk::TCreateInfo toInfo()
        }
    - Result<T, ErrorMessage> TFactory::create(const vk::TCreateInfo& desc)
    - Result<T, ErrorMessage> TFactory::create(const TDescription& desc)
    - static TFactory::defaultDescription()
        provides a default value for a field iff that will be the value in 90% of cases
        advertise which values it *doesn't* set
    */
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