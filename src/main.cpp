#include "utility/object_pool.h"
#include "utility/log/log_utils.h"
#include "utility/log/log_output.h"

using namespace LogUtils;

struct Wisdom
{
    u32 numbers[16];
    Wisdom(u32 num)
    {
        for (u32 i = 0; i < 16; i++)
        {
            this->numbers[i] = num;
        }
    }
};

int main()
{
    std::shared_ptr<Log::LoggingContext> logContext = std::make_shared<Log::LoggingContext>(
        Log::LoggingContext(
            2,
            { "engine", "resources", "vulkan", "game_loop" }
        )
    );
    setGlobalLogger(
        (new Log::Logger(
            logContext,
            std::make_unique<Log::ConsoleOutput>()
        ))->setFilter({ "engine", "resources" })
    );
    getGlobalLogger().filter = ~getGlobalLogger().filter;
    getGlobalLogger().forwardTo(
        std::make_unique<Log::Logger>(
            Log::Logger(
                logContext,
                std::make_unique<Log::FileOutput>(Log::FileOutput("log"))
            )
        )
    );

    log({ .channel = "engine" }, "Life is le beautiful");
    log({ .channel = "engine" }, "Life is le beautiful, {}", "bro");
    getGlobalLogger().log({ .channel = "resources" }, "Life is le beautiful");
    getGlobalLogger().log({ .channel = "resources" }, "Life is le beautiful, {}", "bro");
    warn({ .channel = "vulkan" }, "Warning {}", 1);
    err({ .channel = "vulkan" }, "Err {}", 2);

    Log::BufferedLogMessage msg = getGlobalLogger().context()
        .makeBufferedMessage({ .channel = "vulkan" })
        .setText("I love my {} to the {}.", "little sister", "MAX");
    
    getGlobalLogger().log(std::move(msg));

    // for (u32 i = 0; i < 5; i++)
    // {
    //     pool.free(wisdoms[i]);
    // }
}

// #include "hello_triangle_application/hello_triangle_application.h"

// using namespace HelloTriangleApplication;

// RuntimeConfig getConfig()
// {
//     return RuntimeConfig {
//         .assetBuild = {
//             .buildOption = Setting::BuildOption::ForceBuild
//         }
//     };
// }

// int main()
// {
//     App app(getConfig());
//     app.run();

//     return EXIT_SUCCESS;
// }