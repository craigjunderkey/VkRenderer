#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"
#include "../geometry/buffer.h"
#include "../resources/string_id_map.h"

namespace HelloTriangleApplication { class App; }
namespace HelloTriangleApplication::Geometry { struct Texture; }

namespace HelloTriangleApplication::RenderLoop
{
    struct Material;

    struct MaterialUniformData
    {
        i32 diffMap     alignas(4);
        i32 specMap     alignas(4);
        u32 samplerId   alignas(4);
        f32 shininess   alignas(4);
        bool isUnlit    alignas(4);
        vec2 texOffset  alignas(8);
        vec2 texScale   alignas(8);
        vec3 diffColor  alignas(16);
        vec3 specColor  alignas(16);
    };
    
    class GpuMaterialManager
    {
    private:
        vk::raii::DescriptorSetLayout layout_ = nullptr;
        vk::raii::DescriptorPool pool_ = nullptr;
        vk::raii::DescriptorSet set_ = nullptr;
        Geometry::MappedBuffer buffer_ = {};
        StringIdMap<u32> matIdMap_;

    public:
        static Result<GpuMaterialManager, std::string> create(App& app, u32 numMaterials, u32 numTextures, u32 numSamplers);
        
        Result<Unit, std::string> writeMaterialData(App& app,
            const StringIdMap<Geometry::Texture>& textures, const StringIdMap<Material>& materials);
        
        u32 getMaterialId(const Material& material) const;
        u32 getMaterialId(StringId name) const;

        const vk::raii::DescriptorSetLayout& descriptorSetLayout() const { return this->layout_; }
        const vk::raii::DescriptorSet& descriptorSet() const { return this->set_; }
        const vk::raii::DescriptorPool& descriptorPool() const { return this->pool_; }
        
        u32 numMaterials() const { return this->matIdMap_.size(); }
    };
}