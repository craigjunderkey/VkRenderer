#include "../hello_triangle_application.h"
#include "descriptor_pool.h"
#include "uniform_buffer.h"
#include "../render_loop/cubemap.h"

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, ErrorMessage> createDescriptorPool(App& app)
    {
        std::array<vk::DescriptorPoolSize, 2> poolSizes {
            vk::DescriptorPoolSize {
                .type = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = MAX_FRAMES_IN_FLIGHT
            },
            vk::DescriptorPoolSize {
                .type = vk::DescriptorType::eSampledImage,
                .descriptorCount = MAX_FRAMES_IN_FLIGHT
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
            vk::DescriptorBufferInfo bufferInfo {
                .buffer = *app.uniformBuffers[i].buffer.buffer,
                .offset = 0,
                .range = vk::WholeSize
            };

            vk::DescriptorImageInfo imageInfo {
                .sampler = nullptr,
                .imageView = *app.resources.get<RenderLoop::Cubemap>("skybox").texArray.view,
                .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
            };

            std::array<vk::WriteDescriptorSet, 2> descriptorWrites {
                vk::WriteDescriptorSet {
                    .dstSet = *app.descriptorSets[i],
                    .dstBinding = 0,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = vk::DescriptorType::eUniformBuffer,
                    .pBufferInfo = &bufferInfo
                },
                vk::WriteDescriptorSet {
                    .dstSet = *app.descriptorSets[i],
                    .dstBinding = 1,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = vk::DescriptorType::eSampledImage,
                    .pImageInfo = &imageInfo
                }
            };

            app.device.updateDescriptorSets(descriptorWrites, {});
        }

        return Ok();
    }
}