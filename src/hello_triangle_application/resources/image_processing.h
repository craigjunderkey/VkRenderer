#pragma once
#include "resource_type.h"

namespace HelloTriangleApplication::Resources
{
    class ImageDataExporter : public ResourceExporter
    {
        Result<std::filesystem::path, std::string> exportResource(
            const std::filesystem::path& path, OfflineProcessingData& processingData) override;
    };

    class ImageDataLoader : public ResourceLoader
    {
        Result<Unit, std::string> loadResource(const std::filesystem::path& path,
            std::any resource, RuntimeProcessingData& processingData) override;
    };

    class ImageDataUnloader : public ResourceUnloader
    {
    public:
        Result<Unit, std::string> unloadResource(std::any resource, RuntimeProcessingData& processingData) override;
    };
}