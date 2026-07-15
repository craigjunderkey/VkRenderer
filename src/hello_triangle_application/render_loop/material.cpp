#include "../hello_triangle_application.h"
#include "material.h"

namespace HelloTriangleApplication::RenderLoop
{
    void Material::bind(App& app) const
    {
        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vk::DescriptorImageInfo imageInfo {
                .sampler = *app.resources.samplerManager().sampler(this->texture.sampler),
                .imageView = *this->texture.view,
                .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
            };

            std::array<vk::WriteDescriptorSet, 1> descriptorWrites {
                vk::WriteDescriptorSet {
                    .dstSet = *app.descriptorSets[i],
                    .dstBinding = 1,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                    .pImageInfo = &imageInfo
                }
            };

            app.device.updateDescriptorSets(descriptorWrites, {});
        }
    }
}