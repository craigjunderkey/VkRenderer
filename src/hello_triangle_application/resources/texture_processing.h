#pragma once
#include "resource_type.h"

namespace HelloTriangleApplication::Resources
{
    class TextureExporter : public ResourceExporter
    {
    public:
        Result<Unit, std::string> exportResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override;
    };

    class TextureLinker : public ResourceLinker
    {
    public:
        Result<Unit, std::string> linkResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override;
    };

    class TextureLoader : public ResourceLoader
    {
    public:
        Result<Unit, std::string> loadResource(const std::filesystem::path& path, std::any resource, RuntimeProcessingData& processingData) override;
    };

    class TextureInitializer : public ResourcePostLoadInitializer
    {
    public:
        Result<Unit, std::string> postLoadInitializeResource(std::any resource, RuntimeProcessingData& processingData) override;
    };

    class OfflineTextureTasks : public OfflineResourceTasks
    {
    public:
        Result<Unit, std::string> onBuildStarted(OfflineProcessingData& processingData) override;
        Result<Unit, std::string> onBuildFinished(OfflineProcessingData& processingData) override;
    };

    class RuntimeTextureTasks : public RuntimeResourceTasks
    {
    public:
        Result<Unit, std::string> onStartup(RuntimeProcessingData& processingData) override;
        Result<Unit, std::string> onShutdown(RuntimeProcessingData& processingData) override;
    };
}