#pragma once
#include "../resource_type.h"

namespace VkRenderer::Resources
{
    class TextDataLoader : public ResourceLoader
    {
        Result<Unit, ErrorMessage> loadResource(const std::filesystem::path& path,
            std::any resource, RuntimeProcessingData& processingData) override;
    };

    class JsonDataLoader : public ResourceLoader
    {
        Result<Unit, ErrorMessage> loadResource(const std::filesystem::path& path,
            std::any resource, RuntimeProcessingData& processingData) override;
    };
}