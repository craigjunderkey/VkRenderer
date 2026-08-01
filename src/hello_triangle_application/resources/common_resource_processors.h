#pragma once
#include "resource_type.h"
#include <fstream>
#include <iostream>

namespace HelloTriangleApplication::Resources
{
    class CopyExporter : public ResourceExporter
    {
    private:
        ResourceType& type_;

    public:
        CopyExporter(ResourceType& type) : type_(type) {}

        std::filesystem::path exportResource(const std::filesystem::path& srcPath) override
        {
            std::filesystem::path dstPath = getExportPath(this->type_.resourceDirectory, srcPath);
            std::ifstream src = std::ifstream(srcPath);
            std::ofstream dst = std::ofstream(dstPath);
            dst << src.rdbuf();
        }
    };
}