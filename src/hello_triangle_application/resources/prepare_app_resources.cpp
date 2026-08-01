#include "../hello_triangle_application.h"
#include "prepare_app_resources.h"
#include "resource_types.h"
#include "text_data.h"

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> initializeResourceManager(App& app)
    {
        app.resources2.addResourceType(Types::textData());
        app.resources2.addResourceType(Types::jsonData());
        return Ok();
    }

    Result<Unit, std::string> loadAllResources(App& app)
    {
        ResourceRegistry& registry = app.resources2.registry();
        registry.loadResources<TextData>("text").unwrap();
        registry.loadResources<nlohmann::json>("json").unwrap();

        // test!
        std::cout << registry.get<TextData>("chungus").lines[0] << std::endl;
        std::cout << registry.get<nlohmann::json>("michael")["michael"]["wisdom"] << std::endl;

        return Ok();
    }
}