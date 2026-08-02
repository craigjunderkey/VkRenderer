#include "resource_unloaders.h"
#include "image_data.h"

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> ImageDataUnloader::unloadResource(std::any resource, RuntimeProcessingData& processingData)
    {
        ImageData* imageData = std::any_cast<ImageData*>(resource);
        if (!imageData->pixels)
            return Err<std::string>("Unloading image data " + std::string(imageData->name) + " but pixels == nullptr.");
        free(imageData->pixels);
        return Ok();
    }
}