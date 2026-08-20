#include "utility/object_pool.h"

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
    ObjectPool<Wisdom> pool = ObjectPool<Wisdom>(4, true, "wisdom_pool");
    PoolPtr<Wisdom> wisdoms[5];
    for (u32 i = 0; i < 5; i++)
    {
        wisdoms[i] = pool.alloc(Wisdom(i + 1));
    }
    for (u32 i = 0; i < 5; i++)
    {
        for (u32 j = 0; j < 16; j++)
        {
            std::cout << wisdoms[i]->numbers[j] << " ";
        }
        std::cout << "\n";
    }
    pool.free(wisdoms[3]);
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