#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Geometry
{
    struct MVP
    {
        mat4 model      alignas(16);
        mat4 view       alignas(16);
        mat4 projection alignas(16);
    };

    Result<Unit, std::string> createUniformBuffers(App& app);
    void updateUniformBuffer(App& app, u32 frameIndex);
}