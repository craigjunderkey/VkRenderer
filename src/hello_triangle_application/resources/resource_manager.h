#pragma once
#include "resource_type.h"
#include "asset_conditioning_pipeline.h"
#include "resource_registry.h"
#include "string_id_map.h"

namespace HelloTriangleApplication::Resources
{
    class NewResourceManager
    {
    private:
        std::vector<ResourceType> types_;
        StringIdMap<u32> typeMap_;
        AssetConditioningPipeline conditioningPipeline_;
        ResourceRegistry registry_;
    
    public:
        NewResourceManager()
            : types_({}), conditioningPipeline_(this->types_), registry_({ this->types_, this->typeMap_ })
        {
        }

        AssetConditioningPipeline& conditioningPipeline() { return this->conditioningPipeline_; }
        ResourceRegistry& registry() { return this->registry_; }

        void addResourceType(ResourceType type);
        bool exportExists();
    };
}