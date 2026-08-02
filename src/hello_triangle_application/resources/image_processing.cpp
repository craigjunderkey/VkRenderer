#include "image_processing.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "image_data.h"

namespace HelloTriangleApplication::Resources
{
    Result<ImageData, std::string> loadImage(const std::filesystem::path& path)
    {
        ImageData image;
        image.pixels = stbi_load(
            path.c_str(),
            &image.width,
            &image.height,
            &image.numChannels,
            STBI_rgb_alpha
        );

        if (!image.pixels)
            return Err<std::string>("Failed to load image " + path.string() + ".");

        return std::move(image);
    }

    Result<Unit, std::string> ImageDataLoader::loadResource(const std::filesystem::path& path,
            std::any resource, RuntimeProcessingData& processingData)
    {
        ImageData* imageData = std::any_cast<ImageData*>(resource);
        auto loadResult = loadImage(path);
        CHECK_KEEP(loadResult);
        *imageData = loadResult.take();
        imageData->name = pathToName(path);        
        return Ok();
    }

    Result<Unit, std::string> ImageDataUnloader::unloadResource(std::any resource, RuntimeProcessingData& processingData)
    {
        ImageData* imageData = std::any_cast<ImageData*>(resource);
        if (!imageData->pixels)
            return Err<std::string>("Unloading image data " + std::string(imageData->name) + " but pixels == nullptr.");
        stbi_image_free(imageData->pixels);
        return Ok();
    }

    // NOTE: this exporter is unused since CopyExporter produces quicker load (and build) times
    Result<std::filesystem::path, std::string> ImageDataExporter::exportResource(
        const std::filesystem::path& path, OfflineProcessingData& processingData)
    {
        // load image
        auto loadResult = loadImage(path);
        CHECK_KEEP(loadResult);
        ImageData image = loadResult.take();

        // save it as a .png
        // TODO: replace this with a better png compression library
        std::filesystem::path dstPath = getExportPath("images", path, ".png");
        stbi_write_png(
            dstPath.c_str(),
            image.width,
            image.height,
            image.numChannels, 
            image.pixels,
            image.width * image.numChannels
        );

        return std::move(dstPath);
    }
}