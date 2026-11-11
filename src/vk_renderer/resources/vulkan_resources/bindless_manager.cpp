#include "../../vk_renderer.h"
#include "bindless_manager.h"
#include "../../utilities/vulkan_debug.h"

using namespace VkRenderer::Utilities;

namespace VkRenderer::Resources
{
    const vk::DescriptorBufferInfo* BindlessDescriptorSubmission::tryGetBuffersInfo() const
    {
        if (!this->buffersInfo_.empty())
            return this->buffersInfo_.data();
        else
            return nullptr;
    }

    const vk::DescriptorImageInfo* BindlessDescriptorSubmission::tryGetImagesInfo() const
    {
        if (!this->imagesInfo_.empty())
            return this->imagesInfo_.data();
        else
            return nullptr;
    }

    const vk::BufferView* BindlessDescriptorSubmission::tryGetBufferViews() const
    {
        if (!this->bufferViews_.empty())
            return this->bufferViews_.data();
        else
            return nullptr;
    }
    
    Result<Unit, ErrorMessage> BindlessManager::addBindlessResource(BindlessResource resource, std::vector<StringId> orderedIds)
    {
        if (this->isLayoutBuilt())
        {
            err({ .channel = "resources" }, "If BindlessManager layout is built, then addBindlessResource cannot be called.");
            panic();
        }

        // verify virtual count and vector size match
        std::string resName = resource.name;
        if (orderedIds.size() != resource.virtualCount)
            return ErrorMessage({ .channel = "resources" }, "Ordered StringId vector for bindless_{} has {} elements, but \
resource's virtual count is {}.", resName, orderedIds.size(), resource.virtualCount);

        // add resource and create index table
        this->resources_.emplace_back(std::move(resource));
        CHECK(this->resourceMap_.tryAdd(resName, this->resources_.size() - 1));
        CHECK(this->indexTables_.tryAdd(resName, StringIdMap<u32> { "bindless_index_table_" + resName }));

        // fill index table
        StringIdMap<u32>& indexTable = this->indexTables_.get(resName);
        for (u32 i = 0; i < resource.virtualCount; i++)
        {
            CHECK(indexTable.tryAdd(orderedIds[i], i));
        }

        return Ok();
    }

    Result<Unit, ErrorMessage> BindlessManager::addBuffer(StringId bufferName, Geometry::MappedBuffer&& buffer)
    {
        if (this->isLayoutBuilt())
        {
            err({ .channel = "resources" }, "If BindlessManager layout is built, then addBuffer cannot be called.");
            panic();
        }
        CHECK(this->buffers_.tryAdd(bufferName, std::move(buffer)));
        return Ok();
    }

    Result<Unit, ErrorMessage> BindlessManager::buildLayout()
    {
        if (this->isLayoutBuilt())
        {
            err({ .channel = "resources" }, "BindlessManager layout is already built; can't call buildLayout again.");
            panic();
        }

        // create pool
        {
            std::vector<vk::DescriptorPoolSize> poolSizes;
            poolSizes.reserve(this->resources_.size());
            for (const BindlessResource& res : this->resources_)
            {
                poolSizes.push_back(vk::DescriptorPoolSize {
                    .type = res.descriptorType,
                    .descriptorCount = res.descriptorCount
                });
            }

            vk::DescriptorPoolCreateInfo poolInfo {
                .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                .maxSets = 1,
                .poolSizeCount = static_cast<u32>(poolSizes.size()),
                .pPoolSizes = poolSizes.data()
            };

            auto poolResult = castVkResult(this->app_.device.createDescriptorPool(poolInfo));
            CHECK_KEEP(poolResult);
            this->pool_ = poolResult.take();

            CHECK(setDebugName(this->app_, *this->pool_, "DescriptorPool_Bindless"));
        }

        // create layout
        {
            std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
            layoutBindings.reserve(this->resources_.size());
            for (u32 i = 0; i < this->resources_.size(); i++)
            {
                const BindlessResource& res = this->resources_[i];

                layoutBindings.push_back(vk::DescriptorSetLayoutBinding {
                    .binding = i,
                    .descriptorType = res.descriptorType,
                    .descriptorCount = res.descriptorCount,
                    .stageFlags = res.stages
                });
            }

            vk::DescriptorSetLayoutCreateInfo layoutInfo {
                .bindingCount = static_cast<u32>(layoutBindings.size()),
                .pBindings = layoutBindings.data()
            };

            auto layoutResult = castVkResult(this->app_.device.createDescriptorSetLayout(layoutInfo));
            CHECK_KEEP(layoutResult);
            this->layout_ = layoutResult.take();

            CHECK(setDebugName(this->app_, this->layout_, "DescriptorSetLayout_Bindless"));
        }

        // create descriptor set
        {
            vk::DescriptorSetAllocateInfo allocInfo {
                .descriptorPool = *this->pool_,
                .descriptorSetCount = 1,
                .pSetLayouts = &*this->layout_
            };
            
            auto allocResult = castVkResult(this->app_.device.allocateDescriptorSets(allocInfo));
            CHECK_KEEP(allocResult);
            this->set_ = std::move(allocResult.take().front());

            CHECK(setDebugName(this->app_, this->set_, "DescriptorSet_Bindless"));
        }

        return Ok();
    }

    Result<Unit, ErrorMessage> BindlessManager::load(StringId resName, BindlessDescriptorSubmission submission)
    {
        if (!this->isLayoutBuilt())
        {
            err({ .channel = "resources" }, "BindlessManager layout is not built; can't call load.");
            panic();
        }
        CHECK(this->descriptorSubmissions_.tryAdd(resName, std::move(submission)));
        return Ok();
    }

    void BindlessManager::submitLoadToGpu()
    {
        if (!this->isLayoutBuilt())
        {
            err({ .channel = "resources" }, "BindlessManager layout is not built; can't call submitLoadToGpu.");
            panic();
        }
        
        std::vector<vk::WriteDescriptorSet> writes;
        writes.reserve(this->resources_.size());
        for (u32 i = 0; i < this->resources_.size(); i++)
        {
            const BindlessResource& res = this->resources_[i];
            const BindlessDescriptorSubmission& submission = this->descriptorSubmissions_.get(res.name);
            
            writes.push_back(vk::WriteDescriptorSet {
                .dstSet = *this->set_,
                .dstBinding = i,
                .dstArrayElement = 0,
                .descriptorCount = res.descriptorCount,
                .descriptorType = res.descriptorType,
                .pImageInfo = submission.tryGetImagesInfo(),
                .pBufferInfo = submission.tryGetBuffersInfo(),
                .pTexelBufferView = submission.tryGetBufferViews()
            });
        }

        this->app_.device.updateDescriptorSets(writes, {});
        this->descriptorSubmissions_.clear();
    }

    StringIdMap<u32>& BindlessManager::getIndexTable(StringId resName)
    {
        return this->indexTables_.get(resName);
    }

    const BindlessResource& BindlessManager::getBindlessResource(StringId resName)
    {
        return this->resources_[this->resourceMap_.get(resName)];
    }

    Result<u32, ErrorMessage> BindlessManager::getIndex(StringId resName, StringId id)
    {
        return std::move(this->indexTables_.get(resName).get(id));
    }

    Result<i32, ErrorMessage> BindlessManager::getNullableIndex(StringId resName, StringId id)
    {
        if (id.isNull())
        {
            return -1;
        }
        else
        {
            return std::move(this->indexTables_.get(resName).get(id));
        }
    }

    Geometry::MappedBuffer& BindlessManager::getBuffer(StringId bufferName)
    {
        return this->buffers_.get(bufferName);
    }

    Result<Unit, ErrorMessage> BindlessManager::load(StringId resName, const std::vector<const Geometry::FatBuffer*>& buffers)
    {
        std::vector<vk::DescriptorBufferInfo> info;
        info.reserve(buffers.size());
        for (const Geometry::FatBuffer* buffer : buffers)
        {
            info.push_back(vk::DescriptorBufferInfo {
                .buffer = **buffer,
                .range = buffer->size
            });
        }
        this->load(resName, std::move(info));
        return Ok();
    }

    Result<Unit, ErrorMessage> BindlessManager::load(StringId resName, const std::vector<const Geometry::Texture*>& textures)
    {
        std::vector<vk::DescriptorImageInfo> info;
        info.reserve(textures.size());
        for (const Geometry::Texture* tex : textures)
        {
            info.push_back(vk::DescriptorImageInfo {
                .sampler = nullptr,
                .imageView = *tex->view,
                .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
            });
        }
        this->load(resName, std::move(info));
        return Ok();
    }

    Result<Unit, ErrorMessage> BindlessManager::load(StringId resName, const std::vector<vk::Sampler>& samplers)
    {
        std::vector<vk::DescriptorImageInfo> info;
        info.reserve(samplers.size());
        for (vk::Sampler sampler : samplers)
        {
            info.push_back(vk::DescriptorImageInfo {
                .sampler = sampler,
                .imageView = nullptr,
                .imageLayout = vk::ImageLayout::eUndefined
            });
        }
        this->load(resName, std::move(info));
        return Ok();
    }
}