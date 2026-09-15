#include "image_processing.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "../image_data.h"

namespace VkRenderer::Resources
{
    Result<ImageData, ErrorMessage> loadImage(const std::filesystem::path& path)
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
            return ErrorMessage({ .channel = "resources" }, "Failed to load image {}.", path);

        return std::move(image);
    }

    Result<Unit, ErrorMessage> ImageDataLoader::loadResource(const std::filesystem::path& path,
            std::any resource, RuntimeProcessingData& processingData)
    {
        ImageData* imageData = std::any_cast<ImageData*>(resource);
        auto loadResult = loadImage(path);
        CHECK_KEEP(loadResult);
        *imageData = loadResult.take();
        imageData->name = pathToName(path);        
        return Ok();
    }

    Result<Unit, ErrorMessage> ImageDataUnloader::unloadResource(std::any resource, RuntimeProcessingData& processingData)
    {
        ImageData* imageData = std::any_cast<ImageData*>(resource);
        if (!imageData->pixels)
            return ErrorMessage({ .channel = "resources" }, "Unloading image data {} but pixels == nullptr.", imageData->name);
        stbi_image_free(imageData->pixels);
        return Ok();
    }

    // NOTE: this exporter is unused since CopyExporter produces quicker load (and build) times
    Result<Unit, ErrorMessage> ImageDataExporter::exportResource(
        const std::filesystem::path& path, OfflineProcessingData& processingData)
    {
        // load image
        auto loadResult = loadImage(path);
        CHECK_KEEP(loadResult);
        ImageData image = loadResult.take();

        // save it as a .png
        // TODO: replace this with a better png compression library
        std::filesystem::path dstPath = getExportPath("images", path, ".png");
        CHECK(ImageDataExporter::writePng(dstPath, image));

        return Ok();
    }

    Result<Unit, ErrorMessage> ImageDataExporter::writePng(const std::filesystem::path& dstPath, const ImageData& imageData)
    {
        i32 writeResult = stbi_write_png(
            dstPath.c_str(),
            imageData.width,
            imageData.height,
            imageData.numChannels, 
            imageData.pixels,
            imageData.width * imageData.numChannels
        );
        
        if (!writeResult)
            return ErrorMessage({ .channel = "resources" }, "Failed to write image {}.", imageData.name);

        return Ok();
    }
}