#pragma once
#include "../resource_type.h"

namespace HelloTriangleApplication::Resources
{
    class CubemapExporter : public ResourceExporter
    {
    public:
        Result<Unit, std::string> exportResource(const std::filesystem::path& path, OfflineProcessingData& processingData);
    };

    class CubemapLoader : public ResourceLoader
    {
    public:
        Result<Unit, std::string> loadResource(const std::filesystem::path& path, std::any resource, RuntimeProcessingData& processingData);
    };

    class CubemapInitializer : public ResourcePostLoadInitializer
    {
    public:
        Result<Unit, std::string> postLoadInitializeResource(std::any resource, RuntimeProcessingData& processingData);
    };
}