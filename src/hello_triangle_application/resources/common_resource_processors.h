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

        std::filesystem::path exportResource(const std::filesystem::path& srcPath, OfflineProcessingData& processingData) override
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
        void compileResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override
        {
        }
    };

    class NullLinker : public ResourceLinker
    {
    public:
        void linkResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override
        {
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