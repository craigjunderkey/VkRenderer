#include "descriptor_set_layout.h"
#include "../hello_triangle_application.h"

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, std::string> createDescriptorSetLayout(App& app)
    {
        std::array<vk::DescriptorSetLayoutBinding, 1> bindings {
            vk::DescriptorSetLayoutBinding {
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = 1,
                .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
            }
        };

        vk::DescriptorSetLayoutCreateInfo layoutInfo {
            .bindingCount = (u32)bindings.size(),
            .pBindings = bindings.data()
        };

        TRY_ASSIGN(
            app.descriptorSetLayout,
            castVkResult(app.device.createDescriptorSetLayout(layoutInfo))
        );

        return Ok();
    }
}