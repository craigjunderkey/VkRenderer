#pragma once
#include "../utility/types.h"

namespace HelloTriangleApplication
{
    namespace Setting
    {
        enum AntiAliasing
        {
            None,
            MSAAx2,
            MSAAx4,
            MSAAx8
        };
    }
    
    struct AppConfig
    {
        bool logByDefault;
        Setting::AntiAliasing antiAliasing;
    };

    constexpr AppConfig CONFIG {
        .logByDefault = true,
        .antiAliasing = Setting::AntiAliasing::MSAAx4
    };
}