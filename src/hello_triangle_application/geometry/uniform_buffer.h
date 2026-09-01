#pragma once
#include "../utilities/common.h"
#include "../render_loop/light.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Geometry
{
    struct PerFrameInfo
    {
        mat4 view       alignas(16);
        mat4 projection alignas(16);
        Environment env;
    };

    Result<Unit, ErrorMessage> createPerFrameUbos(App& app);
    Result<Unit, ErrorMessage> updatePerFrameUbo(App& app, u32 frameIndex);
}