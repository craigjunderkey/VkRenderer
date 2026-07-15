#include "../hello_triangle_application.h"
#include "material_manager.h"
#include "../geometry/texture.h"
#include <unordered_set>

using namespace HelloTriangleApplication::Geometry;

std::unordered_map<const vk::raii::Sampler*, u32> makeSamplerIdMap(const std::vector<const vk::raii::Sampler*>& samplers)
{
    auto samplerIdMap = std::unordered_map<const vk::raii::Sampler*, u32>();
    
    for (u32 i = 0; i < samplers.size(); i++)
    {
        samplerIdMap.insert({ samplers[i], i });
    }
    
    return std::move(samplerIdMap);
}

namespace HelloTriangleApplication::RenderLoop
{
    std::vector<const vk::raii::Sampler*> getSamplers(const std::vector<const Texture*>& textures)
    {
        // get the set of samplers
        auto samplers = std::unordered_set<const vk::raii::Sampler*>();
        for (const Texture* tex : textures)
        {
            const vk::raii::Sampler* sampler = tex->sampler;
            samplers.insert(sampler);
        }

        // convert the set to a vector
        std::vector<const vk::raii::Sampler*> samplerVec {};
        samplerVec.reserve(samplers.size());
        for (const vk::raii::Sampler* sampler : samplers)
        {
            samplerVec.push_back(sampler);
        }

        return std::move(samplerVec);
    }

    u32 MaterialManager::getMaterialId(const Material& material) const
    {
        return this->matIdMap_.find(material.name)->second;
    }
    
    u32 MaterialManager::getMaterialId(StringId name) const
    {
        return this->matIdMap_.find(name)->second;
    }

    Result<MaterialManager, std::string> MaterialManager::create(App& app,
        const std::unordered_map<StringId, Texture>& textures, const std::unordered_map<StringId, Material>& materials)
    {
        MaterialManager mats {};

        // make vectors
        std::vector<const Texture*> texVec = mapValuesAsConstPtrs(textures);
        std::vector<const Material*> matVec = mapValuesAsConstPtrs(materials);
        std::vector<const vk::raii::Sampler*> samplerVec = getSamplers(texVec);

        // make maps
        std::unordered_map<StringId, u32> texIdMap = idMapFromKeys(textures);
        std::unordered_map<StringId, u32> matIdMap = idMapFromKeys(materials);
        std::unordered_map<const vk::raii::Sampler*, u32> samplerIdMap = makeSamplerIdMap(samplerVec);

        // debug: print everything!
        println("--Samplers");
        printVecAsOrderedList(samplerVec);
        println("---Ids");
        printMap(samplerIdMap);
        println("--Materials");
        printVecAsOrderedList(matVec);
        println("---Ids");
        printMap(matIdMap);
        println("--Textures");
        printVecAsOrderedList(texVec);
        println("---Ids");
        printMap(texIdMap);

        // MAYBE: what follows is the cause of all the warnings at the end of the program.
        
        // -- TEMPORARY!!!
        // save the StringId to material shader id map for outside use
        mats.matIdMap_ = std::move(matIdMap);

        return std::move(mats);
        // -- TEMPORARY!!!

        // create descriptor pool
        std::array<vk::DescriptorPoolSize, 3> poolSizes {
            vk::DescriptorPoolSize {
                .type = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = (u32)matVec.size()
            },
            vk::DescriptorPoolSize {
                .type = vk::DescriptorType::eSampledImage,
                .descriptorCount = (u32)texVec.size()
            },
            vk::DescriptorPoolSize {
                .type = vk::DescriptorType::eSampler,
                .descriptorCount = (u32)samplerVec.size()
            }
        };
        vk::DescriptorPoolCreateInfo poolInfo {
            .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            .maxSets = 1,
            .poolSizeCount = (u32)poolSizes.size(),
            .pPoolSizes = poolSizes.data()
        };
        TRY_ASSIGN(mats.pool_, castVkResult(app.device.createDescriptorPool(poolInfo)));

        // create descriptor layout
        std::array<vk::DescriptorSetLayoutBinding, 3> layoutBindings {
            vk::DescriptorSetLayoutBinding {
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = (u32)matVec.size(),
                .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
            },
            vk::DescriptorSetLayoutBinding {
                .binding = 1,
                .descriptorType = vk::DescriptorType::eSampledImage,
                .descriptorCount = (u32)texVec.size(),
                .stageFlags = vk::ShaderStageFlagBits::eFragment
            },
            vk::DescriptorSetLayoutBinding {
                .binding = 2,
                .descriptorType = vk::DescriptorType::eSampler,
                .descriptorCount = (u32)samplerVec.size(),
                .stageFlags = vk::ShaderStageFlagBits::eFragment
            }
        };
        vk::DescriptorSetLayoutCreateInfo layoutInfo {
            .bindingCount = (u32)layoutBindings.size(),
            .pBindings = layoutBindings.data()
        };
        TRY_ASSIGN(mats.layout_, castVkResult(app.device.createDescriptorSetLayout(layoutInfo)));

        // allocate descriptor set
        vk::DescriptorSetAllocateInfo allocInfo {
            .descriptorPool = *mats.pool_,
            .descriptorSetCount = 1,
            .pSetLayouts = &*mats.layout_
        };
        auto allocResult = castVkResult(app.device.allocateDescriptorSets(allocInfo));
        CHECK_KEEP(allocResult);
        mats.set_ = std::move(allocResult.take().front());

        // construct array of MaterialUniformData
        std::vector<MaterialUniformData> matData {};
        matData.reserve(matVec.size());
        for (u32 i = 0; i < matVec.size(); i++)
        {
            matData.push_back(MaterialUniformData {
                .texId = texIdMap.find(matVec[i]->texture.name)->second,
                .samplerId = samplerIdMap.find(&*matVec[i]->texture.sampler)->second,
                .shininess = matVec[i]->shininess,
                .isUnlit = matVec[i]->isUnlit,
                .texOffset = matVec[i]->texOffset,
                .texScale = matVec[i]->texScale,
                .ambient = matVec[i]->ambient,
                .diffuse = matVec[i]->diffuse,
                .specular = matVec[i]->specular
            });
        }

        // populate material buffer        
        u64 matBufferSize = sizeof(MaterialUniformData) * matData.size();
        TRY_ASSIGN(
            mats.buffer_,
            MappedBuffer::fromFatBufferResult(
                FatBuffer::create(
                    app,
                    matBufferSize,
                    vk::BufferUsageFlagBits::eUniformBuffer,
                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                )
            )
        );
        memcpy(mats.buffer_.data, &matData[0], matBufferSize);

        // construct descriptor infos
        vk::DescriptorBufferInfo matBufferInfo {
            .buffer = *mats.buffer_.buffer,
            .offset = 0,
            .range = vk::WholeSize
        };

        std::vector<vk::DescriptorImageInfo> texImageInfo {};
        texImageInfo.reserve(texVec.size());
        for (u32 i = 0; i < texVec.size(); i++)
        {
            texImageInfo.push_back(vk::DescriptorImageInfo {
                .sampler = nullptr,
                .imageView = *texVec[i]->view,
                .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
            });
        }

        std::vector<vk::DescriptorImageInfo> samplerImageInfo {};
        samplerImageInfo.reserve(samplerVec.size());
        for (u32 i = 0; i < samplerVec.size(); i++)
        {
            samplerImageInfo.push_back(vk::DescriptorImageInfo {
                .sampler = **samplerVec[i],
                .imageView = nullptr,
                .imageLayout = vk::ImageLayout::eUndefined
            });
        }

        // populate descriptor set
        std::array<vk::WriteDescriptorSet, 3> writeDescriptorSet {
            vk::WriteDescriptorSet {
                .dstSet = *mats.set_,
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .pBufferInfo = &matBufferInfo
            },
            vk::WriteDescriptorSet {
                .dstSet = *mats.set_,
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = (u32)texImageInfo.size(),
                .descriptorType = vk::DescriptorType::eSampledImage,
                .pImageInfo = texImageInfo.data()
            },
            vk::WriteDescriptorSet {
                .dstSet = *mats.set_,
                .dstBinding = 2,
                .dstArrayElement = 0,
                .descriptorCount = (u32)samplerImageInfo.size(),
                .descriptorType = vk::DescriptorType::eSampler,
                .pImageInfo = samplerImageInfo.data()
            }
        };

        app.device.updateDescriptorSets(writeDescriptorSet, {});
        
        // save the StringId to material shader id map for outside use
        mats.matIdMap_ = std::move(matIdMap);

        return std::move(mats);
    }
}