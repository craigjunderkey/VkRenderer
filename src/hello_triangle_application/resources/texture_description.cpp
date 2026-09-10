#include "texture_description.h"

namespace VkRenderer::Resources
{
    Result<Unit, ErrorMessage> TextureDescription::check(const StringIdMap<ImageData>& images) const
    {
        // check `dimensions`
        if (this->image.isNull() && !this->dimensions)
            return ErrorMessage({ .channel = "resources" }, "If `image` is null, then `dimensions` must be set.");
        
        // check `additionalImages`
        if (this->image.isNull() && !this->additionalImages.empty())
            return ErrorMessage({ .channel = "resources" }, "If `image` is null, then `additionalImages` must be empty.");
        
        if (!this->image.isNull())
        {
            const ImageData& imageData = images.get(this->image);
            for (StringId layer : this->additionalImages)
            {
                const ImageData& layerData = images.get(layer);
                bool matchesDimensions = imageData.width == layerData.width
                    && imageData.height == layerData.height;
                if (!matchesDimensions)
                    return ErrorMessage({ .channel = "resources" }, "All images in `additionalImages` must have the same dimensions as `image`.");
            }
        }

        // check `numLayers`
        u32 imagesFound = 0;
        if (!this->image.isNull())
            imagesFound++;
        imagesFound += this->additionalImages.size();
        if (imagesFound > numLayers)
            return ErrorMessage({ .channel = "resources" }, "`numLayers` <= the number of images supplied.");

        return Ok();
    }

    TextureDescription TextureDescription::defaultTexture()
    {
        return TextureDescription {
            .dimensions = {},
            .imageType = vk::ImageType::e2D,
            .viewType = vk::ImageViewType::e2D,
            .doMipmapping = true,
            .doMultisampling = false,
            .format = vk::Format::eR8G8B8A8Srgb,
            .tiling = vk::ImageTiling::eOptimal,
            .usageFlags = vk::ImageUsageFlagBits::eTransferSrc
                | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            .memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
            .aspectFlags = vk::ImageAspectFlagBits::eColor,
            .numLayers = 1,
            .additionalImages = {}
        };
    }

    TextureDescription TextureDescription::screenTexture(vk::Extent2D screen)
    {
        return TextureDescription {
            .image = StringId::NONE,
            .dimensions = vk::Extent3D { screen.width, screen.height, 1 },
            .imageType = vk::ImageType::e2D,
            .viewType = vk::ImageViewType::e2D,
            .doMipmapping = false,
            .doMultisampling = true,
            .tiling = vk::ImageTiling::eOptimal,
            .memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
            .numLayers = 1,
            .additionalImages = {}
        };
    }

    TextureDescription& TextureDescriptionManager::getOrAdd(StringId texture)
    {
        Option<TextureDescription*> desc = this->descs_.tryGet(texture);
        if (desc)
        {
            return *desc.value();
        }
        else
        {
            this->descs_.add(texture, TextureDescription::defaultTexture());
            return this->descs_.get(texture);
        }
    }

    Result<Unit, ErrorMessage> TextureDescriptionManager::drop(StringId texture)
    {
        auto removeDescResult = this->descs_.tryRemove(texture);
        auto removePathResult = this->paths_.tryRemove(texture);
        if (!removeDescResult)
            return ErrorMessage({ .channel = "resources" }, "Tried to drop {} from TextureDescriptionsManager, but couldn't find it.", texture);
        
        if (removePathResult)
        {
            std::filesystem::path path = std::move(removePathResult).value();
            std::filesystem::remove(path);
        }

        return Ok();
    }

    Result<Unit, ErrorMessage> TextureDescriptionManager::attachPath(StringId texture, std::filesystem::path path)
    {
        if (!this->descs_.tryGet(texture))
            return ErrorMessage({ .channel = "resources" }, "Tried to attach path to {} but it's not in the description map.", texture);
        CHECK(this->paths_.tryAdd(texture, std::move(path)));
        return Ok();
    }
}