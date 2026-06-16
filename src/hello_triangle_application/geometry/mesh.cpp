#include "../hello_triangle_application.h"
#include "mesh.h"

namespace HelloTriangleApplication::Geometry
{
    std::vector<u32> meshQueueFamilies(App& app)
    {
        return { app.graphicsQueue.familyIndex, app.transferQueue.familyIndex };
    }
}