#pragma once
#include "../resource_type.h"

namespace VkRenderer::Resources
{
    class CubemapExporter : public ResourceExporter
    {
    public:
        Result<Unit, ErrorMessage> exportResource(const std::filesystem::path& path, OfflineProcessingData& processingData);
    };

    class CubemapLoader : public ResourceLoader
    {
    public:
        Result<Unit, ErrorMessage> loadResource(const std::filesystem::path& path, std::any resource, RuntimeProcessingData& processingData);
    };

    class CubemapInitializer : public ResourcePostLoadInitializer
    {
    public:
        Result<Unit, ErrorMessage> postLoadInitializeResource(std::any resource, RuntimeProcessingData& processingData);
    };
}