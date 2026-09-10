#pragma once
#include "../utility/types.h"

namespace VkRenderer
{
    namespace Setting
    {
        enum class BuildOption
        {
            ForceBuild,
            BuildIfNone,
            NoBuild
        };

        enum class AntiAliasing
        {
            None,
            MSAAx2,
            MSAAx4,
            MSAAx8
        };

        namespace Group
        {
            struct AssetBuild
            {
                BuildOption buildOption;
            };

            struct GraphicsSettings
            {
                AntiAliasing antiAliasing;
            };
        }
    }

    struct Config
    {
        Setting::Group::AssetBuild build;
        Setting::Group::GraphicsSettings graphics;
    };
}