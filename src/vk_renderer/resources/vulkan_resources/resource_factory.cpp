#include "resource_factory.h"

namespace VkRenderer::Resources
{
    VulkanResourceFactory::VulkanResourceFactory(VulkanResourceContext context)
        : context_(context)
    {
        this->factoryImageViewFactory_.setContext(this->context_);
    }
}