#include "../hello_triangle_application.h"
#include "prepare_app_resources.h"
#include "resource_types.h"
#include "text_data.h"
#include "image_data.h"

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> initializeResourceManager(App& app)
    {
        app.resources2.addResourceType(Types::textData());
        app.resources2.addResourceType(Types::jsonData());
        app.resources2.addResourceType(Types::imageData());
        return Ok();
    }

    Result<Unit, std::string> loadAllResources(App& app)
    {
        ResourceRegistry& registry = app.resources2.registry();
        registry.loadResources<TextData>("text").unwrap();
        registry.loadResources<nlohmann::json>("json").unwrap();
        registry.loadResources<ImageData>("image").unwrap();
        return Ok();
    }
}