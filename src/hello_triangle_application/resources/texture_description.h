#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"
#include "string_id_map.h"
#include "image_data.h"

namespace HelloTriangleApplication::Resources
{
    struct TextureDescription
    {
        StringId image = StringId::NONE;
        vk::ImageType imageType;
        vk::ImageViewType viewType;
        bool doMipmapping;
        bool doMultisampling;
        vk::Format format;
        vk::ImageTiling tiling;
        vk::ImageUsageFlags usage;
        vk::MemoryPropertyFlags properties;
        vk::ImageAspectFlags aspectFlags;

        // `numLayers` <= the number of images supplied
        u32 numLayers;

        // all images in `additionalImages` must have the same dimensions as `image`
        // if `image` is null, then `additionalImages` must be empty
        std::vector<StringId> additionalImages = {}; 

        Result<Unit, std::string> check(const StringIdMap<ImageData>& images);
        
        static TextureDescription defaultTexture();
    };

    class TextureDescriptionManager
    {
    private:
        StringIdMap<TextureDescription> descs_;
    
    public:
        const StringIdMap<TextureDescription>& descs() const { return this->descs_; }
        StringIdMap<TextureDescription>& descs() { return this->descs_; }
    
        TextureDescription& getOrAdd(StringId texture)
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
    };
}