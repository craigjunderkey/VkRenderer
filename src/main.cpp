#include "utility/log.h"

int main()
{
    log("global", 0, "I love my {}! The amount? {}.", "wife", "maximum");
}

// #include "hello_triangle_application/hello_triangle_application.h"

// using namespace HelloTriangleApplication;

// RuntimeConfig getConfig()
// {
//     return RuntimeConfig {
//         .assetBuild = {
//             .buildOption = Setting::BuildOption::ForceBuild,
//             .doBuildLogging = true
//         }
//     };
// }

// int main()
// {
//     App app(getConfig());
//     app.run();

//     return EXIT_SUCCESS;
// }