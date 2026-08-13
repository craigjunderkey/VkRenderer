#include "texture_description.h"

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> TextureDescription::check(const StringIdMap<ImageData>& images)
    {
        // check `additionalImages`
        if (this->image.isNull() && !this->additionalImages.empty())
            return Err<std::string>("If `image` is null, then `additionalImages` must be empty.");
        
        const ImageData& imageData = images.get(this->image);
        for (StringId layer : this->additionalImages)
        {
            const ImageData& layerData = images.get(layer);
            bool matchesDimensions = imageData.width == layerData.width
                && imageData.height == layerData.height;
            if (!matchesDimensions)
                return Err<std::string>("All images in `additionalImages` must have the same dimensions as `image`.");
        }

        // check `numLayers`
        u32 imagesFound = 0;
        if (!this->image.isNull())
            imagesFound++;
        imagesFound += this->additionalImages.size();
        if (imagesFound > numLayers)
            return Err<std::string>("`numLayers` <= the number of images supplied.");

        return Ok();
    }

    TextureDescription TextureDescription::defaultTexture()
    {
        return TextureDescription
        {
            .imageType = vk::ImageType::e2D,
            .viewType = vk::ImageViewType::e2D,
            .doMipmapping = true,
            .doMultisampling = false,
            .format = vk::Format::eR8G8B8A8Srgb,
            .tiling = vk::ImageTiling::eOptimal,
            .usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            .aspectFlags = vk::ImageAspectFlagBits::eColor,
            .numLayers = 1
        };
    }
}