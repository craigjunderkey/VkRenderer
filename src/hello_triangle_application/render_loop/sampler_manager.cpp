#include "../hello_triangle_application.h"
#include "sampler_manager.h"
#include "../geometry/texture.h"

namespace HelloTriangleApplication::RenderLoop
{
    Result<Unit, std::string> SamplerManager::loadSamplers(App& app)
    {
        vk::PhysicalDeviceProperties deviceProperties = app.physicalDevice.getProperties();

        // make default sampler
        {
            auto samplerResult = Texture::makeDefaultSampler(app);
            if (!samplerResult)
                return Err<std::string>("Failed to create texture sampler.");
            
            this->samplers_[SamplerName::Default] = samplerResult.take();
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
}
