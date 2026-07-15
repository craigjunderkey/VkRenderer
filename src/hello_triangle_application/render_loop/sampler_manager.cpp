#include "../hello_triangle_application.h"
#include "sampler_manager.h"

namespace HelloTriangleApplication::RenderLoop
{
    Result<Unit, std::string> SamplerManager::loadSamplers(App& app)
    {
        vk::PhysicalDeviceProperties deviceProperties = app.physicalDevice.getProperties();

        // make default sampler
        {
            vk::SamplerCreateInfo samplerInfo {
                .magFilter = vk::Filter::eLinear,
                .minFilter = vk::Filter::eLinear,
                .mipmapMode = vk::SamplerMipmapMode::eLinear,
                .addressModeU = vk::SamplerAddressMode::eMirroredRepeat,
                .addressModeV = vk::SamplerAddressMode::eMirroredRepeat,
                .addressModeW = vk::SamplerAddressMode::eMirroredRepeat,
                .mipLodBias = 0.0f,
                .anisotropyEnable = vk::True,
                .maxAnisotropy = deviceProperties.limits.maxSamplerAnisotropy,
                .compareEnable = vk::False,
                .compareOp = vk::CompareOp::eAlways,
                .minLod = 0.0f,
                .maxLod = 0.0f,
                .borderColor = vk::BorderColor::eIntOpaqueBlack,
                .unnormalizedCoordinates = vk::False
            };

            auto samplerResult = castVkResult(app.device.createSampler(samplerInfo));
            if (!samplerResult)
                return Err<std::string>("Failed to create texture sampler.");
            
            this->samplerIdMap_.insert({ StringId(), samplerResult.take() });
        }

        return Ok();
    }

    const vk::raii::Sampler& SamplerManager::sampler(StringId name)
    {
        return this->samplerIdMap_.find(name)->second;
    }

    u32 SamplerManager::numSamplers() const
    {
        // TODO: treat samplers as a resource; keep a vector of, say, SamplerResourceDatas
        return 1;
    }
}
