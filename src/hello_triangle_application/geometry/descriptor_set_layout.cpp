#include "descriptor_set_layout.h"
#include "../hello_triangle_application.h"

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, std::string> createDescriptorSetLayout(App& app)
    {
        vk::DescriptorSetLayoutBinding uboBinding {
            .binding = 0,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 1,
            .stageFlags = vk::ShaderStageFlagBits::eVertex
        };

        vk::DescriptorSetLayoutCreateInfo layoutInfo {
            .bindingCount = 1,
            .pBindings = &uboBinding
        };

        TRY_ASSIGN(
            app.descriptorSetLayout,
            castVkResult(app.device.createDescriptorSetLayout(layoutInfo))
        );

        return Ok();
    }
}