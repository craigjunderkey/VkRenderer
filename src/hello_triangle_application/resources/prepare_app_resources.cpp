#include "../hello_triangle_application.h"
#include "prepare_app_resources.h"
#include "resource_types.h"
#include "text_data.h"
#include "image_data.h"
#include "../geometry/mesh.h"
#include "../render_loop/material.h"
#include "../render_loop/cubemap.h"

#include <chrono>

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> initializeResourceManager(App& app)
    {
        app.resources.addResourceType(Types::textData());
        app.resources.addResourceType(Types::jsonData());
        app.resources.addResourceType(Types::imageData());
        app.resources.addResourceType(Types::mesh());
        app.resources.addResourceType(Types::material());
        app.resources.addResourceType(Types::texture());
        app.resources.addResourceType(Types::shader());
        app.resources.addResourceType(Types::cubemap());
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
        LogLoader loader(app.resources.registry());

        loader.load<TextData>("text").unwrap();
        loader.load<nlohmann::json>("json").unwrap();
        loader.load<ImageData>("image").unwrap();
        loader.load<Geometry::Mesh>("mesh").unwrap();
        loader.load<RenderLoop::Material>("material").unwrap();
        loader.load<Geometry::Texture>("texture").unwrap();
        loader.load<Shader>("shader").unwrap();
        loader.load<RenderLoop::Cubemap>("cubemap").unwrap();

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
                shouldBuild = !app.resources.exportExists();
                break;
            case Setting::BuildOption::NoBuild:
                shouldBuild = false;
                break;
        }

        if (shouldBuild)
        {
            CHECK(app.resources.conditioningPipeline().buildAssets(App::RUNTIME_CONFIG.assetBuild.doBuildLogging));
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
        CHECK(app.resources.registry().postLoadInitialize<Geometry::Mesh>("mesh"));
        CHECK(app.resources.registry().postLoadInitialize<Geometry::Texture>("texture"));
        CHECK(app.resources.registry().postLoadInitialize<RenderLoop::Material>("material"));
        CHECK(app.resources.registry().postLoadInitialize<RenderLoop::Cubemap>("cubemap"));
        
        CHECK(app.resources.registry().unloadResources<ImageData>("image"));

        return Ok();
    }

    Result<Unit, std::string> handlePrePipelineInitialization(App& app)
    {
        // make samplers
        CHECK(app.samplers.makeSamplers(app));
        
        // make GPU material manager
        auto gpuMatResult = GpuMaterialManager::create(
            app,
            app.resources.registry().getMap<RenderLoop::Material>().size(),
            app.resources.registry().getMap<Texture>().size(),
            app.samplers.numSamplers()
        );
        CHECK_KEEP(gpuMatResult);
        app.gpuMaterials = gpuMatResult.take();

        return Ok();
    }
}