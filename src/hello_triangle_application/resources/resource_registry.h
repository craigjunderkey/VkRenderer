#pragma once
#include "../utilities/common.h"
#include "resource_type.h"
#include "string_id_map.h"

namespace HelloTriangleApplication::Resources
{
    class ResourceRegistry
    {
    private:
        const std::vector<ResourceType>& types_;

    public:
        ResourceRegistry(const std::vector<ResourceType>& types) : types_(types) {}
        Result<Unit, std::string> loadAssets();
    };
}