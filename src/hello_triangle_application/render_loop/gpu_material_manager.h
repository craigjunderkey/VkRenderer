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