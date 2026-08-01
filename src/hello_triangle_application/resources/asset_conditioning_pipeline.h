#pragma once
#include "../utilities/common.h"
#include "resource_type.h"

namespace HelloTriangleApplication::Resources
{
    class AssetConditioningPipeline
    {
    private:
        const std::vector<ResourceType>& types_;
    
    public:
        AssetConditioningPipeline(const std::vector<ResourceType>& types) : types_(types) {}
        Result<Unit, std::string> conditionAssets();
    };
}