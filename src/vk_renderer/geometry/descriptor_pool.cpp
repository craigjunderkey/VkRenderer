#include "../vk_renderer.h"
#include "descriptor_pool.h"
#include "uniform_buffer.h"
#include "../render_loop/cubemap.h"

namespace VkRenderer::Geometry
{
    Result<Unit, ErrorMessage> createDescriptorPool(App& app)
    {
        std::array<vk::DescriptorPoolSize, 2> poolSizes {
            vk::DescriptorPoolSize {
                .type = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = MAX_FRAMES_IN_FLIGHT
            },
            vk::DescriptorPoolSize {
                .type = vk::DescriptorType::eStorageBuffer,
                .descriptorCount = 2 * MAX_FRAMES_IN_FLIGHT
            }
        };

        vk::DescriptorPoolCreateInfo poolInfo {
            .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            .maxSets = MAX_FRAMES_IN_FLIGHT,
            .poolSizeCount = (u32)poolSizes.size(),
            .pPoolSizes = poolSizes.data()
        };

        auto poolResult = castVkResult(app.device.createDescriptorPool(poolInfo));
        CHECK_KEEP(poolResult);
        app.descriptorPool = poolResult.take();

        return Ok();
    }

    Result<Unit, ErrorMessage> createDescriptorSets(App& app)
    {
        std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *app.descriptorSetLayout);
        
        vk::DescriptorSetAllocateInfo allocInfo {
            .descriptorPool = *app.descriptorPool,
            .descriptorSetCount = (u32)layouts.size(),
            .pSetLayouts = layouts.data()
        };

        auto allocSetsResult = castVkResult(app.device.allocateDescriptorSets(allocInfo));
        CHECK_KEEP(allocSetsResult);
        app.descriptorSets = allocSetsResult.take();

        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vk::DescriptorBufferInfo uboInfo {
                .buffer = *app.uniformBuffers[i].buffer.buffer,
                .offset = 0,
                .range = vk::WholeSize
            };

            vk::DescriptorBufferInfo currParticles {
                .buffer = *app.particleSystem.buffers[i],
                .offset = 0,
                .range = vk::WholeSize
            };
            
            vk::DescriptorBufferInfo prevParticles {
                .buffer = *app.particleSystem.buffers[(i - 1) % MAX_FRAMES_IN_FLIGHT],
                .offset = 0,
                .range = vk::WholeSize
            };

            std::array<vk::WriteDescriptorSet, 3> descriptorWrites {
                vk::WriteDescriptorSet {
                    .dstSet = *app.descriptorSets[i],
                    .dstBinding = 0,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = vk::DescriptorType::eUniformBuffer,
                    .pBufferInfo = &uboInfo
                },
                vk::WriteDescriptorSet {
                    .dstSet = *app.descriptorSets[i],
                    .dstBinding = 1,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = vk::DescriptorType::eStorageBuffer,
                    .pBufferInfo = &currParticles,
                },
                vk::WriteDescriptorSet {
                    .dstSet = *app.descriptorSets[i],
                    .dstBinding = 1,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = vk::DescriptorType::eStorageBuffer,
                    .pBufferInfo = &prevParticles,
                }
            };

            app.device.updateDescriptorSets(descriptorWrites, {});
        }

        return Ok();
    }
}