#include "../hello_triangle_application.h"
#include "descriptor_pool.h"
#include "uniform_buffer.h"

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, std::string> createDescriptorPool(App& app)
    {
        std::array<vk::DescriptorPoolSize, 1> poolSizes {
            vk::DescriptorPoolSize {
                .type = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = MAX_FRAMES_IN_FLIGHT
            }
        };

        vk::DescriptorPoolCreateInfo poolInfo {
            .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            .maxSets = MAX_FRAMES_IN_FLIGHT,
            .poolSizeCount = (u32)poolSizes.size(),
            .pPoolSizes = poolSizes.data()
        };

        TRY_ASSIGN(
            app.descriptorPool,
            castVkResult(app.device.createDescriptorPool(poolInfo))
        );

        return Ok();
    }

    Result<Unit, std::string> createDescriptorSets(App& app)
    {
        std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *app.descriptorSetLayout);
        
        vk::DescriptorSetAllocateInfo allocInfo {
            .descriptorPool = *app.descriptorPool,
            .descriptorSetCount = (u32)layouts.size(),
            .pSetLayouts = layouts.data()
        };

        TRY_ASSIGN(
            app.descriptorSets,
            castVkResult(app.device.allocateDescriptorSets(allocInfo))
        );

        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vk::DescriptorBufferInfo bufferInfo {
                .buffer = *app.uniformBuffers[i].buffer.buffer,
                .offset = 0,
                .range = vk::WholeSize
            };

            std::array<vk::WriteDescriptorSet, 1> descriptorWrites {
                vk::WriteDescriptorSet {
                    .dstSet = *app.descriptorSets[i],
                    .dstBinding = 0,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = vk::DescriptorType::eUniformBuffer,
                    .pBufferInfo = &bufferInfo
                }
            };

            app.device.updateDescriptorSets(descriptorWrites, {});
        }

        return Ok();
    }
}