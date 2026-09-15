#pragma once
#include "../resource_type.h"
#include "../image_data.h"

namespace VkRenderer::Resources
{
    class ImageDataExporter : public ResourceExporter
    {
    public:
        Result<Unit, ErrorMessage> exportResource(
            const std::filesystem::path& path, OfflineProcessingData& processingData) override;
        static Result<Unit, ErrorMessage> writePng(const std::filesystem::path& dstPath, const ImageData& imageData);
    };

    class ImageDataLoader : public ResourceLoader
    {
    public:
        Result<Unit, ErrorMessage> loadResource(const std::filesystem::path& path,
            std::any resource, RuntimeProcessingData& processingData) override;
    };

    class ImageDataUnloader : public ResourceUnloader
    {
    public:
        Result<Unit, ErrorMessage> unloadResource(std::any resource, RuntimeProcessingData& processingData) override;
    };
}