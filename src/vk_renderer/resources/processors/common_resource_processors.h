#pragma once
#include "../resource_type.h"
#include <fstream>
#include <iostream>

namespace VkRenderer::Resources
{
    class CopyExporter : public ResourceExporter
    {
    private:
        const ResourceType* type_;

    public:
        CopyExporter() : type_(nullptr) {}
        CopyExporter(const ResourceType& type) : type_(&type) {}

        Result<Unit, ErrorMessage> exportResource(const std::filesystem::path& srcPath, OfflineProcessingData& processingData) override
        {
            std::filesystem::path dstPath = getExportPath(this->type_->resourceDirectory, srcPath);
            std::ifstream src = std::ifstream(srcPath, std::ios::binary);
            std::ofstream dst = std::ofstream(dstPath, std::ios::binary);
            dst << src.rdbuf();
            return Ok();
        }
    };

    class NullCompiler : public ResourceCompiler
    {
    public:
        Result<Unit, ErrorMessage> compileResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override
        {
            return Ok();
        }
    };

    class NullLinker : public ResourceLinker
    {
    public:
        Result<Unit, ErrorMessage> linkResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override
        {
            return Ok();
        }
    };

    class NullOfflineTasks : public OfflineResourceTasks
    {
    public:
        Result<Unit, ErrorMessage> onBuildStarted(OfflineProcessingData& processingData) override
        {
            return Ok();
        }
        Result<Unit, ErrorMessage> onBuildFinished(OfflineProcessingData& processingData) override
        {
            return Ok();
        }
    };

    class NullUnloader : public ResourceUnloader
    {
    public:
        Result<Unit, ErrorMessage> unloadResource(std::any resource, RuntimeProcessingData& processingData) override
        {
            return Ok();
        }
    };

    class NullPostLoadInitializer : public ResourcePostLoadInitializer
    {
    public:
        Result<Unit, ErrorMessage> postLoadInitializeResource(std::any resource, RuntimeProcessingData& processingData) override
        {
            return ErrorMessage({ .channel = "resources" },
                "Post-load initialized a resource with no defined post-load initialization behavior.");
        }
    };

    class NullRuntimeTasks : public RuntimeResourceTasks
    {
    public:
        Result<Unit, ErrorMessage> onStartup(RuntimeProcessingData& processingData) override
        {
            return Ok();
        }
        Result<Unit, ErrorMessage> onShutdown(RuntimeProcessingData& processingData) override
        {
            return Ok();
        }
    };
}