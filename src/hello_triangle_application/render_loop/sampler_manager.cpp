#include "../hello_triangle_application.h"
#include "sampler_manager.h"
#include "../geometry/texture.h"

namespace HelloTriangleApplication::RenderLoop
{
    Result<Unit, ErrorMessage> SamplerManager::makeSamplers(App& app)
    {
        // make repeat sampler
        {
            vk::SamplerCreateInfo samplerInfo = Texture::defaultSamplerInfo(app);
            samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
            samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
            samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;

            auto samplerResult = castVkResult(app.device.createSampler(samplerInfo));
            if (!samplerResult)
                return ErrorMessage({ .channel = "vulkan "}, "Failed to create repeat sampler.");

            this->samplers_[SamplerName::Repeat] = samplerResult.take();
        }

        // make mirrored repeat sampler
        {
            vk::SamplerCreateInfo samplerInfo = Texture::defaultSamplerInfo(app);
            samplerInfo.addressModeU = vk::SamplerAddressMode::eMirroredRepeat;
            samplerInfo.addressModeV = vk::SamplerAddressMode::eMirroredRepeat;
            samplerInfo.addressModeW = vk::SamplerAddressMode::eMirroredRepeat;

            auto samplerResult = castVkResult(app.device.createSampler(samplerInfo));
            if (!samplerResult)
                return ErrorMessage({ .channel = "vulkan" }, "Failed to create mirrored repeat sampler.");

            this->samplers_[SamplerName::MirroredRepeat] = samplerResult.take();
        }

        // make clamped sampler
        {
            vk::SamplerCreateInfo samplerInfo = Texture::defaultSamplerInfo(app);
            samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
            samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
            samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;

            auto samplerResult = castVkResult(app.device.createSampler(samplerInfo));
            if (!samplerResult)
                return ErrorMessage({ .channel = "vulkan" }, "Failed to create clamped sampler.");
            
            this->samplers_[SamplerName::Clamped] = samplerResult.take();
        }

        return Ok();
    }

    const vk::raii::Sampler& SamplerManager::sampler(SamplerName name) const
    {
        return this->samplers_[name];
    }

    const vk::raii::Sampler& SamplerManager::sampler(u32 id) const
    {
        assert(id < SamplerName::None);
        return this->samplers_[id];
    }

    u32 SamplerManager::numSamplers() const
    {
        return SamplerName::None;
    }

    SamplerName SamplerManager::samplerNameFromString(std::string_view str)
    {
        if (str.compare("repeat") == 0)
        {
            return SamplerName::Repeat;
        }
        else if (str.compare("mirrored_repeat") == 0)
        {
            return SamplerName::MirroredRepeat;
        }
        else if (str.compare("clamped") == 0)
        {
            return SamplerName::Clamped;
        }
        else
        {
            return SamplerName::None;
        }
    }
}
