#include "../hello_triangle_application.h"
#include "prepare_app_resources.h"
#include "resource_types.h"
#include "text_data.h"
#include "image_data.h"
#include "../geometry/mesh.h"

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> initializeResourceManager(App& app)
    {
        app.resources2.addResourceType(Types::textData());
        app.resources2.addResourceType(Types::jsonData());
        app.resources2.addResourceType(Types::imageData());
        app.resources2.addResourceType(Types::mesh());
        return Ok();
    }

    Result<Unit, std::string> loadAllResources(App& app)
    {
        ResourceRegistry& registry = app.resources2.registry();
        registry.loadResources<TextData>("text").unwrap();
        registry.loadResources<nlohmann::json>("json").unwrap();
        registry.loadResources<ImageData>("image").unwrap();
        registry.loadResources<Geometry::Mesh>("mesh").unwrap();
        return Ok();
    }

    Result<Unit, std::string> handleAssetBuild(App& app)
    {
        bool shouldBuild;

        switch (App::RUNTIME_CONFIG.assetBuild)
        {
            case Setting::AssetBuild::ForceBuild:
                shouldBuild = true;
                break;
            case Setting::AssetBuild::BuildIfNone:
                shouldBuild = !app.resources2.exportExists();
                break;
            case Setting::AssetBuild::NoBuild:
                shouldBuild = false;
                break;
        }

        if (shouldBuild)
        {
            std::cout << "Building assets\n";
            CHECK(app.resources2.conditioningPipeline().buildAssets());
        }
        else
        {
            std::cout << "Skipping asset build\n";
        }

        return Ok();
    }
}