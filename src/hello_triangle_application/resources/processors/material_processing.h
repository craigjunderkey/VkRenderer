#pragma once
#include "../resource_type.h"

namespace HelloTriangleApplication::Resources
{
    class MaterialExporter : public ResourceExporter
    {
    public:
        Result<Unit, ErrorMessage> exportResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override;
    };

    class MaterialLoader : public ResourceLoader
    {
    public:
        Result<Unit, ErrorMessage> loadResource(const std::filesystem::path& path, std::any resource, RuntimeProcessingData& processingData) override;
    };

    class MaterialInitializer : public ResourcePostLoadInitializer
    {
    public:
        Result<Unit, ErrorMessage> begin(RuntimeProcessingData& processingData) override;
        Result<Unit, ErrorMessage> end(RuntimeProcessingData& processingData) override;
        Result<Unit, ErrorMessage> postLoadInitializeResource(std::any resource, RuntimeProcessingData& processingData) override;
    };
}