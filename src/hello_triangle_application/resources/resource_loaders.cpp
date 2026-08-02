#include "resource_loaders.h"
#include <iostream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "text_data.h"
#include <nlohmann/json.hpp>
#include "image_data.h"

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> TextDataLoader::loadResource(const std::filesystem::path& path,
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

    Result<Unit, std::string> JsonDataLoader::loadResource(const std::filesystem::path& path,
        std::any resource, RuntimeProcessingData& processingData)
    {
        nlohmann::json* jsonData = std::any_cast<nlohmann::json*>(resource);
        {
            std::ifstream file = std::ifstream(path);
            *jsonData = nlohmann::json::parse(file);
        }

        return Ok();
    }

    Result<Unit, std::string> ImageDataLoader::loadResource(const std::filesystem::path& path,
            std::any resource, RuntimeProcessingData& processingData)
    {
        ImageData* imageData = std::any_cast<ImageData*>(resource);
        imageData->name = pathToName(path);
        imageData->pixels = stbi_load(
            path.c_str(),
            &imageData->width,
            &imageData->height,
            &imageData->numChannels,
            STBI_rgb_alpha
        );

        if (!imageData->pixels)
            return Err<std::string>("Failed to load image " + path.string() + ".");
        
        return Ok();
    }
}