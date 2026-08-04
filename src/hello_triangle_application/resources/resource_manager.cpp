#include "../hello_triangle_application.h"
#include "resource_manager.h"
#include <filesystem>

namespace HelloTriangleApplication::Resources
{
    NewResourceManager::NewResourceManager(App& app) :
        types_({}),
        conditioningPipeline_(this->types_),
        registry_({ this->types_, this->typeMap_ })
    {
        this->registry_.processingData() = RuntimeProcessingData {
            .app = &app
        };
    }

    void NewResourceManager::addResourceType(ResourceType type)
    {
        type.isValid().unwrap();
        this->typeMap_.add(StringId(type.resourceName), this->types_.size());
        this->types_.emplace_back(std::move(type));
    }

    bool NewResourceManager::exportExists()
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