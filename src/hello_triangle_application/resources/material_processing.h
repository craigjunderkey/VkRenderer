#pragma once
#include "resource_type.h"

namespace HelloTriangleApplication::Resources
{
    class MaterialExporter : public ResourceExporter
    {
    public:
        Result<Unit, std::string> exportResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override;
    };

    class MaterialLoader : public ResourceLoader
    {
    public:
        Result<Unit, std::string> loadResource(const std::filesystem::path& path, std::any resource, RuntimeProcessingData& processingData) override;
    };

    class MaterialInitializer : public ResourcePostLoadInitializer
    {
    public:
        Result<Unit, std::string> begin(RuntimeProcessingData& processingData) override;
        Result<Unit, std::string> end(RuntimeProcessingData& processingData) override;
        Result<Unit, std::string> postLoadInitializeResource(std::any resource, RuntimeProcessingData& processingData) override;
    };
}