#pragma once
#include "../resource_type.h"

namespace VkRenderer::Resources
{
    class TextureExporter : public ResourceExporter
    {
    public:
        Result<Unit, ErrorMessage> exportResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override;
    };

    class TextureLinker : public ResourceLinker
    {
    public:
        Result<Unit, ErrorMessage> linkResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override;
    };

    class TextureLoader : public ResourceLoader
    {
    public:
        Result<Unit, ErrorMessage> loadResource(const std::filesystem::path& path, std::any resource, RuntimeProcessingData& processingData) override;
    };

    class TextureInitializer : public ResourcePostLoadInitializer
    {
    public:
        Result<Unit, ErrorMessage> postLoadInitializeResource(std::any resource, RuntimeProcessingData& processingData) override;
        Result<Unit, ErrorMessage> end(RuntimeProcessingData& processingData) override;
    };

    class OfflineTextureTasks : public OfflineResourceTasks
    {
    public:
        Result<Unit, ErrorMessage> onBuildStarted(OfflineProcessingData& processingData) override;
        Result<Unit, ErrorMessage> onBuildFinished(OfflineProcessingData& processingData) override;
    };

    class RuntimeTextureTasks : public RuntimeResourceTasks
    {
    public:
        Result<Unit, ErrorMessage> onStartup(RuntimeProcessingData& processingData) override;
        Result<Unit, ErrorMessage> onShutdown(RuntimeProcessingData& processingData) override;
    };
}