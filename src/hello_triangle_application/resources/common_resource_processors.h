#pragma once
#include "resource_type.h"
#include <fstream>
#include <iostream>

namespace HelloTriangleApplication::Resources
{
    class CopyExporter : public ResourceExporter
    {
    private:
        const ResourceType* type_;

    public:
        CopyExporter() : type_(nullptr) {}
        CopyExporter(const ResourceType& type) : type_(&type) {}

        Result<std::filesystem::path, std::string> exportResource(const std::filesystem::path& srcPath, OfflineProcessingData& processingData) override
        {
            std::filesystem::path dstPath = getExportPath(this->type_->resourceDirectory, srcPath);
            std::ifstream src = std::ifstream(srcPath);
            std::ofstream dst = std::ofstream(dstPath);
            dst << src.rdbuf();
            return std::move(dstPath);
        }
    };

    class NullCompiler : public ResourceCompiler
    {
    public:
        Result<Unit, std::string> compileResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override
        {
            return Ok();
        }
    };

    class NullLinker : public ResourceLinker
    {
    public:
        Result<Unit, std::string> linkResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override
        {
            return Ok();
        }
    };

    class NullUnloader : public ResourceUnloader
    {
    public:
        Result<Unit, std::string> unloadResource(std::any resource, RuntimeProcessingData& processingData) override
        {
            return Ok();
        }
    };
}