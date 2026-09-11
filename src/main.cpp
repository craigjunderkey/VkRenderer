#include "vk_renderer/vk_renderer.h"

using namespace VkRenderer;

Config getConfig()
{
    return Config {
        .build = {
            .buildOption = Setting::BuildOption::ForceBuild
        },
        .graphics = {
            .antiAliasing = Setting::AntiAliasing::MSAAx4
        }
    };
}

int main()
{
    App app(getConfig());
    app.run();

    return EXIT_SUCCESS;
}