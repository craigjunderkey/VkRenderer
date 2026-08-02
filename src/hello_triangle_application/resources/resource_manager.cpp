#include "resource_manager.h"

namespace HelloTriangleApplication::Resources
{
    void NewResourceManager::addResourceType(ResourceType type)
    {
        type.isValid().unwrap();
        this->typeMap_.add(StringId(type.resourceName), this->types_.size());
        this->types_.emplace_back(std::move(type));
    }
}