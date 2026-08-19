#include "hello_triangle_application/hello_triangle_application.h"

using namespace HelloTriangleApplication;

RuntimeConfig getConfig()
{
    return RuntimeConfig {
        .assetBuild = {
            .buildOption = Setting::BuildOption::ForceBuild
        }
    };
}

int main()
{
    App app(getConfig());
    app.run();

    return EXIT_SUCCESS;
}