#pragma once
#include "../../utilities/common.h"
#include "../../utilities/vulkan_common.h"
#include "../../../utility/string_id_map.h"
#include "../../geometry/buffer.h"

#include <variant>

namespace VkRenderer { class App; }
namespace VkRenderer::Geometry { class Texture; }

namespace VkRenderer::Resources
{
    struct BindlessResource
    {
        const char* name;
        
        // the number of virtual, named, symbolic instances of this resource that exist
        // e.g., one buffer might represent several instances
        u32 virtualCount;

        // the number of Vulkan descriptors this resource comprises
        u32 descriptorCount;

        vk::DescriptorType descriptorType;
        vk::ShaderStageFlags stages;
    };

    struct BindlessDescriptorSubmission
    {
    private:
        std::vector<vk::DescriptorImageInfo> imagesInfo_;
        std::vector<vk::DescriptorBufferInfo> buffersInfo_;
        std::vector<vk::BufferView> bufferViews_;

    public:
        BindlessDescriptorSubmission() = default;
        BindlessDescriptorSubmission(std::vector<vk::DescriptorImageInfo>&& imagesInfo)
            : imagesInfo_(std::move(imagesInfo)) {};
        BindlessDescriptorSubmission(std::vector<vk::DescriptorBufferInfo>&& buffersInfo)
            : buffersInfo_(std::move(buffersInfo)) {};
        BindlessDescriptorSubmission(std::vector<vk::BufferView>&& bufferViews)
            : bufferViews_(std::move(bufferViews)) {};
        
        const vk::DescriptorImageInfo* tryGetImagesInfo() const;
        const vk::DescriptorBufferInfo* tryGetBuffersInfo() const;
        const vk::BufferView* tryGetBufferViews() const;
    };

    class BindlessManager
    {
    private:
        App& app_;

        std::vector<BindlessResource> resources_ {};
    
        StringIdMap<StringIdMap<u32>> indexTables_ { "bindless_index_tables" };
        StringIdMap<u32> resourceMap_ { "bindless_resource_map" };
        StringIdMap<BindlessDescriptorSubmission> descriptorSubmissions_ { "bindless_descriptor_submissions" };
        StringIdMap<Geometry::MappedBuffer> buffers_ { "bindless_buffer_table" };

        vk::raii::DescriptorSetLayout layout_ = nullptr;
        vk::raii::DescriptorPool pool_ = nullptr;
        vk::raii::DescriptorSet set_ = nullptr;

    public:
        BindlessManager(App& app) : app_(app) {}

        inline bool isLayoutBuilt() const { return this->layout_ != nullptr; }
        inline vk::raii::DescriptorSetLayout& getDescriptorSetLayout() { return this->layout_; }
        inline vk::raii::DescriptorPool& getDescriptorPool() { return this->pool_; }
        inline vk::raii::DescriptorSet& getDescriptorSet() { return this->set_; }
        
        Result<Unit, ErrorMessage> addBindlessResource(BindlessResource resource, std::vector<StringId> orderedIds);
        Result<Unit, ErrorMessage> addBuffer(StringId bufferName, Geometry::MappedBuffer&& buffer);
        Result<Unit, ErrorMessage> buildLayout();

        StringIdMap<u32>& getIndexTable(StringId resName);
        const BindlessResource& getBindlessResource(StringId resName);
        Result<u32, ErrorMessage> getIndex(StringId resName, StringId id);
        Result<i32, ErrorMessage> getNullableIndex(StringId resName, StringId id);
        Geometry::MappedBuffer& getBuffer(StringId bufferName);

        Result<Unit, ErrorMessage> load(StringId resName, BindlessDescriptorSubmission submission);
        
        Result<Unit, ErrorMessage> load(StringId resName, const std::vector<const Geometry::FatBuffer*>& buffers);
        Result<Unit, ErrorMessage> load(StringId resName, const std::vector<const Geometry::Texture*>& textures);
        Result<Unit, ErrorMessage> load(StringId resName, const std::vector<vk::Sampler>& samplers);

        void submitLoadToGpu();
    };
}