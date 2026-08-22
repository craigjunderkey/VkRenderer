#include "hello_triangle_application/hello_triangle_application.h"

using namespace HelloTriangleApplication;

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