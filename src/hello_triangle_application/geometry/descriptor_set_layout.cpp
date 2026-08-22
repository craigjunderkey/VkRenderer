#include "descriptor_set_layout.h"
#include "../hello_triangle_application.h"

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, ErrorMessage> createDescriptorSetLayout(App& app)
    {
        std::array<vk::DescriptorSetLayoutBinding, 2> bindings {
            vk::DescriptorSetLayoutBinding {
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = 1,
                .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
            },
            vk::DescriptorSetLayoutBinding {
                .binding = 1,
                .descriptorType = vk::DescriptorType::eSampledImage,
                .descriptorCount = 1,
                .stageFlags = vk::ShaderStageFlagBits::eFragment
            }
        };

        vk::DescriptorSetLayoutCreateInfo layoutInfo {
            .bindingCount = (u32)bindings.size(),
            .pBindings = bindings.data()
        };

        auto layoutResult = castVkResult(app.device.createDescriptorSetLayout(layoutInfo));
        CHECK_KEEP(layoutResult);
        app.descriptorSetLayout = layoutResult.take();

        return Ok();
    }
}