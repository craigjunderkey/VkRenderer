#pragma once
#include "../utility/types.h"

namespace HelloTriangleApplication
{
    struct AppConfig
    {
        bool logByDefault;
    };

    constexpr AppConfig CONFIG {
        .logByDefault = true
    };
}