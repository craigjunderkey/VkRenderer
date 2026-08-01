#include "../hello_triangle_application.h"
#include "prepare_app_resources.h"
#include "resource_text_data.h"
#include "resource_json_data.h"

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> initializeResourceManager(App& app)
    {
        app.resources2.addResourceType(Types::textData());
        app.resources2.addResourceType(Types::jsonData());
        return Ok();
    }
}