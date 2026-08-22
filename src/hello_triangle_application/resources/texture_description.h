#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"
#include "../../utility/string_id_map.h"
#include "image_data.h"
#include <filesystem>

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Resources
{
    struct TextureDescription
    {
        StringId image = StringId::NONE;

        // if `image` is null, then `dimensions` must be set
        Option<vk::Extent3D> dimensions = {};
        
        vk::ImageType imageType;
        vk::ImageViewType viewType;
        bool doMipmapping;
        bool doMultisampling;
        vk::Format format;
        vk::ImageTiling tiling;
        vk::ImageUsageFlags usageFlags;
        vk::ImageCreateFlags createFlags;
        vk::MemoryPropertyFlags memoryProperties;
        vk::ImageAspectFlags aspectFlags;

        // `numLayers` <= the number of images supplied
        u32 numLayers;

        // all images in `additionalImages` must have the same dimensions as `image`
        // if `image` is null, then `additionalImages` must be empty
        std::vector<StringId> additionalImages = {}; 

        Result<Unit, ErrorMessage> check(const StringIdMap<ImageData>& images) const;
        
        static TextureDescription defaultTexture();

        // leaves only the following unset: `format`, `usageFlags`, and `aspectFlags`
        static TextureDescription screenTexture(vk::Extent2D screenDimensions);
    };

    class TextureExporter;
    
    class TextureDescriptionManager
    {   
    private:
        StringIdMap<TextureDescription> descs_ { "texture_descriptions" };
        StringIdMap<std::filesystem::path> paths_ { "texture_description_path_map" };
    
    public:
        const StringIdMap<TextureDescription>& descs() const { return this->descs_; }
        StringIdMap<TextureDescription>& descs() { return this->descs_; }
    
        TextureDescription& getOrAdd(StringId texture);
        Result<Unit, ErrorMessage> drop(StringId texture);
        Result<Unit, ErrorMessage> attachPath(StringId texture, std::filesystem::path path);
    };
}