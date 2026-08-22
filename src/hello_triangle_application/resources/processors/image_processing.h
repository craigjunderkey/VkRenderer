#pragma once
#include "../resource_type.h"

namespace HelloTriangleApplication::Resources
{
    class ImageDataExporter : public ResourceExporter
    {
        Result<Unit, ErrorMessage> exportResource(
            const std::filesystem::path& path, OfflineProcessingData& processingData) override;
    };

    class ImageDataLoader : public ResourceLoader
    {
        Result<Unit, ErrorMessage> loadResource(const std::filesystem::path& path,
            std::any resource, RuntimeProcessingData& processingData) override;
    };

    class ImageDataUnloader : public ResourceUnloader
    {
    public:
        Result<Unit, ErrorMessage> unloadResource(std::any resource, RuntimeProcessingData& processingData) override;
    };
}