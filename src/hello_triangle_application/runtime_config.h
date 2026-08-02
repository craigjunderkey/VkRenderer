#pragma once
#include "../utility/types.h"

namespace HelloTriangleApplication
{
    namespace Setting
    {
        enum class AssetBuild
        {
            ForceBuild,
            BuildIfNone,
            NoBuild
        };
    }

    struct RuntimeConfig
    {
        Setting::AssetBuild assetBuild;
    };
}