#include "../hello_triangle_application.h"
#include "prepare_app_resources.h"
#include "resource_types.h"
#include "text_data.h"
#include "image_data.h"
#include "../geometry/mesh.h"
#include "../render_loop/material.h"

#include <chrono>

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> initializeResourceManager(App& app)
    {
        app.resources2.addResourceType(Types::textData());
        app.resources2.addResourceType(Types::jsonData());
        app.resources2.addResourceType(Types::imageData());
        app.resources2.addResourceType(Types::mesh());
        app.resources2.addResourceType(Types::material());
        return Ok();
    }

    class LogLoader
    {
    private:
        ResourceRegistry& registry_;
    
    public:
        LogLoader(ResourceRegistry& registry) : registry_(registry)
        {
            std::cout << "Loading resources\n";
        }

        template <typename T>
        Result<Unit, std::string> load(StringId resourceName)
        {
            auto start = std::chrono::high_resolution_clock::now();
            CHECK(this->registry_.loadResources<T>(resourceName));
            auto stop = std::chrono::high_resolution_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            std::cout << "\t" << resourceName.string() << "\t\t" << this->registry_.getMap<T>().size() << "\t" << ms << std::endl;
            return Ok();
        }
    };

    Result<Unit, std::string> loadAllResources(App& app)
    {
        LogLoader loader(app.resources2.registry());

        loader.load<TextData>("text").unwrap();
        loader.load<nlohmann::json>("json").unwrap();
        loader.load<ImageData>("image").unwrap();
        loader.load<Geometry::Mesh>("mesh").unwrap();
        loader.load<RenderLoop::Material>("material").unwrap();
        
        return Ok();
    }

    Result<Unit, std::string> handleAssetBuild(App& app)
    {
        bool shouldBuild;

        switch (App::RUNTIME_CONFIG.assetBuild.buildOption)
        {
            case Setting::BuildOption::ForceBuild:
                shouldBuild = true;
                break;
            case Setting::BuildOption::BuildIfNone:
                shouldBuild = !app.resources2.exportExists();
                break;
            case Setting::BuildOption::NoBuild:
                shouldBuild = false;
                break;
        }

        if (shouldBuild)
        {
            CHECK(app.resources2.conditioningPipeline().buildAssets(App::RUNTIME_CONFIG.assetBuild.doBuildLogging));
        }
        else
        {
            if (App::RUNTIME_CONFIG.assetBuild.doBuildLogging)
                std::cout << "Skipped asset build\n";
        }

        return Ok();
    }

    Result<Unit, std::string> handlePostLoadInitialization(App& app)
    {
        CHECK(app.resources2.registry().postLoadInitialize<Geometry::Mesh>("mesh"));
        CHECK(app.resources2.registry().postLoadInitialize<RenderLoop::Material>("material"));
        return Ok();
    }
}