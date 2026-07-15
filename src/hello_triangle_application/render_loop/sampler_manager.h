#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"

#include <unordered_map>

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::RenderLoop
{
    class SamplerManager
    {
    private:
        std::unordered_map<StringId, vk::raii::Sampler> samplerIdMap_;
    
    public:
        Result<Unit, std::string> loadSamplers(App& app);
        const vk::raii::Sampler& sampler(StringId name);
        u32 numSamplers() const;
    };
}