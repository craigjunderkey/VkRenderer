#include "../hello_triangle_application.h"
#include "resource_manager.h"
#include <filesystem>

namespace VkRenderer::Resources
{
    ResourceManager::ResourceManager(App& app) :
        types_({}),
        conditioningPipeline_(this->types_),
        registry_({ this->types_, this->typeMap_ }),
        bindless_(app)
    {
        this->registry_.processingData() = RuntimeProcessingData {
            .app = &app
        };
    }

    void ResourceManager::addResourceType(ResourceType type)
    {
        type.check().unwrap();
        this->typeMap_.add(StringId(type.resourceName), this->types_.size());
        this->types_.emplace_back(std::move(type));
    }

    bool ResourceManager::exportExists()
    {
        if (!std::filesystem::exists(RES_EXPORT_DIR))
            return false;
        
        if (!std::filesystem::is_directory(RES_EXPORT_DIR))
            return false;
        
        if (std::filesystem::is_empty(RES_EXPORT_DIR))
            return false;
        
        return true;
    }
}