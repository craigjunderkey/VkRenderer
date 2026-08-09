#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::RenderLoop
{
    enum SamplerName
    {
        Repeat,
        MirroredRepeat,
        None
    };

    class SamplerManager
    {
    private:
        std::array<vk::raii::Sampler, SamplerName::None> samplers_ { nullptr, nullptr };
    
    public:
        Result<Unit, std::string> makeSamplers(App& app);
        const vk::raii::Sampler& sampler(SamplerName name) const;
        const vk::raii::Sampler& sampler(u32 id) const;
        u32 numSamplers() const;

        static SamplerName samplerNameFromString(const char* str);
    };
}