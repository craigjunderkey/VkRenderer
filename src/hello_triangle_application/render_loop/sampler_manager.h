#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::RenderLoop
{
    enum SamplerName
    {
        Default,
        Count
    };

    class SamplerManager
    {
    private:
        std::array<vk::raii::Sampler, SamplerName::Count> samplers_ {nullptr};
    
    public:
        Result<Unit, std::string> loadSamplers(App& app);
        const vk::raii::Sampler& sampler(SamplerName name) const;
        u32 numSamplers() const;
    };
}