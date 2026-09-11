#pragma once
#include "resource_type_factory.h"
#include <concepts>
#include "factories/image_view_factory.h"
#include "factories/semaphore_factory.h"
#include "factories/fence_factory.h"

#define REGISTER_FACTORY_TYPE(type)             \
    private:                                    \
        type factory##type##_;                  \
    public:                                     \
        template<>                              \
        const type & get< type >()              \
        {                                       \
            return factory##type##_;            \
        }

namespace VkRenderer::Resources
{
    class VulkanResourceFactory
    {
    private:
        VulkanResourceContext context_;
    
    public:
        VulkanResourceFactory() = delete;
        VulkanResourceFactory(VulkanResourceContext context);

        template<typename T> requires std::derived_from<T, VulkanResourceTypeFactory>
        const T& get()
        {
            err({ .channel = "resources" }, "Vulkan resource factory type does not exist.");
            panic();
        }

        REGISTER_FACTORY_TYPE(ImageViewFactory)
        REGISTER_FACTORY_TYPE(SemaphoreFactory)
        REGISTER_FACTORY_TYPE(FenceFactory)
    };
}