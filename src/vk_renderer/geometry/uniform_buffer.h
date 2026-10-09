#pragma once
#include "../utilities/common.h"
#include "../render_loop/light.h"

namespace VkRenderer { class App; }

namespace VkRenderer::Geometry
{
    struct PerFrameInfo
    {
        mat4 view       alignas(16);
        mat4 projection alignas(16);
        Environment env;
        float dt;
    };

    Result<Unit, ErrorMessage> createPerFrameUbos(App& app);
    Result<Unit, ErrorMessage> updatePerFrameUbo(App& app, u32 frameIndex);
}