#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"
#include "../geometry/buffer.h"

#include <unordered_map>

namespace HelloTriangleApplication { class App; }
namespace HelloTriangleApplication::Geometry { struct Texture; }

namespace HelloTriangleApplication::RenderLoop
{
    struct Material;

    struct MaterialUniformData
    {
        u32 texId       alignas(4);
        u32 samplerId   alignas(4);
        f32 shininess   alignas(4);
        bool isUnlit    alignas(4);
        vec2 texOffset  alignas(8);
        vec2 texScale   alignas(8);
        vec3 ambient    alignas(16);
        vec3 diffuse    alignas(16);
        vec3 specular   alignas(16);
    };
    
    class MaterialManager
    {
    private:
        vk::raii::DescriptorSetLayout layout_ = nullptr;
        vk::raii::DescriptorSet set_ = nullptr;
        vk::raii::DescriptorPool pool_ = nullptr;
        Geometry::MappedBuffer buffer_ = {};
        std::unordered_map<StringId, u32> matIdMap_;

    public:
        static Result<MaterialManager, std::string> create(App& app,
            const std::unordered_map<StringId, Geometry::Texture>& textures, const std::unordered_map<StringId, Material>& materials);
        
        u32 getMaterialId(const Material& material) const;
        u32 getMaterialId(StringId name) const;

        const vk::raii::DescriptorSet& descriptorSet() const { return this->set_; }
    };
}