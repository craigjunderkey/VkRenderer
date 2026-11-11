#include "descriptor_set_layout.h"
#include "../vk_renderer.h"
#include "../utilities/vulkan_debug.h"

using namespace VkRenderer::Utilities;

namespace VkRenderer::Geometry
{
    Result<Unit, ErrorMessage> createDescriptorSetLayout(App& app)
    {
        std::array<vk::DescriptorSetLayoutBinding, 3> bindings {
            vk::DescriptorSetLayoutBinding {
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = 1,
                .stageFlags = vk::ShaderStageFlagBits::eVertex
                    | vk::ShaderStageFlagBits::eFragment
                    | vk::ShaderStageFlagBits::eCompute
            },
            vk::DescriptorSetLayoutBinding {
                .binding = 1,
                .descriptorType = vk::DescriptorType::eStorageBuffer,
                .descriptorCount = 1,
                .stageFlags = vk::ShaderStageFlagBits::eCompute
            },
            vk::DescriptorSetLayoutBinding {
                .binding = 2,
                .descriptorType = vk::DescriptorType::eStorageBuffer,
                .descriptorCount = 1,
                .stageFlags = vk::ShaderStageFlagBits::eCompute
            }
        };

        vk::DescriptorSetLayoutCreateInfo layoutInfo {
            .bindingCount = (u32)bindings.size(),
            .pBindings = bindings.data()
        };

        auto layoutResult = castVkResult(app.device.createDescriptorSetLayout(layoutInfo));
        CHECK_KEEP(layoutResult);
        app.descriptorSetLayout = layoutResult.take();

        CHECK(setDebugName(app, *app.descriptorSetLayout, "DescriptorSetLayout_PerFrameUbo"));

        return Ok();
    }
}