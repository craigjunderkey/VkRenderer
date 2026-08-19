#pragma once
#include "../utility/types.h"

namespace HelloTriangleApplication
{
    namespace Setting
    {
        enum class BuildOption
        {
            ForceBuild,
            BuildIfNone,
            NoBuild
        };

        struct AssetBuild
        {
            BuildOption buildOption;
        };
    }

    struct RuntimeConfig
    {
        Setting::AssetBuild assetBuild;
    };
}