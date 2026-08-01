#pragma once
#include "resource_type.h"
#include "asset_conditioning_pipeline.h"
#include "resource_registry.h"

namespace HelloTriangleApplication::Resources
{
    class NewResourceManager
    {
    private:
        std::vector<ResourceType> types_;
        AssetConditioningPipeline conditioningPipeline_;
        ResourceRegistry registry_;
    
    public:
        NewResourceManager()
            : types_({}), conditioningPipeline_(this->types_), registry_(this->types_)
        {
        }

        AssetConditioningPipeline& conditioningPipeline() { return this->conditioningPipeline_; }
        ResourceRegistry& registry() { return this->registry_; }

        void addResourceType(ResourceType type)
        {
            this->types_.emplace_back(std::move(type));
        }
    };
}