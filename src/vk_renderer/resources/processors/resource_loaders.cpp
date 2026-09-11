#include "resource_loaders.h"
#include <iostream>
#include <fstream>

#include "../text_data.h"
#include <nlohmann/json.hpp>

namespace VkRenderer::Resources
{
    Result<Unit, ErrorMessage> TextDataLoader::loadResource(const std::filesystem::path& path,
        std::any resource, RuntimeProcessingData& processingData)
    {
        TextData* textData = std::any_cast<TextData*>(resource);
        std::vector<std::string>& lines = textData->lines;
        std::ifstream in = std::ifstream(path);

        lines = {};
        while (!in.eof())
        {
            std::string line;
            getline(in, line);
            lines.emplace_back(std::move(line));
        }

        return Ok();
    }

    Result<Unit, ErrorMessage> JsonDataLoader::loadResource(const std::filesystem::path& path,
        std::any resource, RuntimeProcessingData& processingData)
    {
        nlohmann::json* jsonData = std::any_cast<nlohmann::json*>(resource);
        {
            std::ifstream file = std::ifstream(path);
            *jsonData = nlohmann::json::parse(file);
        }

        return Ok();
    }
}