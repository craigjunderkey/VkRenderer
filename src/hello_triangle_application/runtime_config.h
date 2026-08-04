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
            bool doBuildLogging;
        };
    }

    struct RuntimeConfig
    {
        Setting::AssetBuild assetBuild;
        bool logAssetBuild;
    };
}