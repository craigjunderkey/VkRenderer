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
        app.resources.addResourceType(Types::cubemap());
        app.resources.addResourceType(Types::shader());
        return Ok();
    }

    class LogLoader
    {
    private:
        ResourceRegistry& registry_;
        std::stringstream loadOut_;
    
    public:
        LogLoader(ResourceRegistry& registry) : registry_(registry)
        {
            log({ .channel = "resources" }, "Loading resources");
            loadOut_ << "\n";
        }

        template <typename T>
        Result<Unit, std::string> load(StringId resourceName)
        {
            auto start = std::chrono::high_resolution_clock::now();
            CHECK(this->registry_.loadResources<T>(resourceName));
            auto stop = std::chrono::high_resolution_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            loadOut_ << "\t" << resourceName.string() << "\t\t" << this->registry_.getMap<T>().size() << "\t" << ms << std::endl;
            return Ok();
        }

        ~LogLoader()
        {
            log({ .channel = "resources" }, this->loadOut_.str());
        }
    };

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
            CHECK(app.resources.conditioningPipeline().buildAssets());
        }
        else
        {
            log({ .channel = "engine" }, "Skipped asset build");
        }

        return Ok();
    }

    Result<Unit, std::string> loadAllResources(App& app)
    {
        LogLoader loader(app.resources.registry());

        loader.load<TextData>("text").unwrap();
        loader.load<nlohmann::json>("json").unwrap();
        loader.load<ImageData>("image").unwrap();
        loader.load<Geometry::Mesh>("mesh").unwrap();
        loader.load<RenderLoop::Material>("material").unwrap();
        loader.load<Geometry::Texture>("texture").unwrap();
        loader.load<RenderLoop::Cubemap>("cubemap").unwrap();
        loader.load<Shader>("shader").unwrap();

        return Ok();
    }

    Result<Unit ,std::string> addBindlessResources(App& app)
    {
        BindlessManager& bindless = app.resources.bindless();
        
        // material
        std::vector<StringId> matIdVec = app.resources.registry().getMap<RenderLoop::Material>().makeIdVec();
        std::sort(matIdVec.begin(), matIdVec.end());
        bindless.addBindlessResource(
            BindlessResource {
                .name = "material",
                .virtualCount = static_cast<u32>(matIdVec.size()),
                .descriptorCount = 1,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .stages = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
            },
            std::move(matIdVec)
        );
        
        // texture
        std::vector<StringId> texIdVec = app.resources.registry().getMap<Geometry::Texture>().makeIdVec();
        std::sort(texIdVec.begin(), texIdVec.end());
        bindless.addBindlessResource(
            BindlessResource {
                .name = "texture",
                .virtualCount = static_cast<u32>(texIdVec.size()),
                .descriptorCount = static_cast<u32>(texIdVec.size()),
                .descriptorType = vk::DescriptorType::eSampledImage,
                .stages = vk::ShaderStageFlagBits::eFragment
            },
            std::move(texIdVec)
        );

        // cubemap
        std::vector<StringId> cubeIdVec = app.resources.registry().getMap<RenderLoop::Cubemap>().makeIdVec();
        std::sort(cubeIdVec.begin(), cubeIdVec.end());
        bindless.addBindlessResource(
            BindlessResource {
                .name = "cubemap",
                .virtualCount = static_cast<u32>(cubeIdVec.size()),
                .descriptorCount = static_cast<u32>(cubeIdVec.size()),
                .descriptorType = vk::DescriptorType::eSampledImage,
                .stages = vk::ShaderStageFlagBits::eFragment
            },
            std::move(cubeIdVec)
        );

        // sampler
        std::vector<StringId> samplerIdVec = { "repeat", "mirrored_repeat", "clamped" }; // TODO: replace this when samplers become a resource
        bindless.addBindlessResource(
            BindlessResource {
                .name = "sampler",
                .virtualCount = static_cast<u32>(samplerIdVec.size()),
                .descriptorCount = static_cast<u32>(samplerIdVec.size()),
                .stages = vk::ShaderStageFlagBits::eFragment
            },
            std::move(samplerIdVec)
        );

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
        
        // add material buffer
        auto matBufferResult = MappedBuffer::fromFatBufferResult(
            FatBuffer::create(
                app,
                app.resources.bindless().getBindlessResource("material").virtualCount * sizeof(RenderLoop::MaterialUniformData),
                vk::BufferUsageFlagBits::eUniformBuffer,
                vk::MemoryPropertyFlagBits::eDeviceLocal | vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
            )
        );
        CHECK_KEEP(matBufferResult);
        MappedBuffer matBuffer = matBufferResult.take();
        app.resources.bindless().addBuffer("materials", std::move(matBuffer));

        // build bindless manager layout
        CHECK(app.resources.bindless().buildLayout());

        return Ok();
    }

    Result<Unit, std::string> loadAndSubmitBindlessResources(App& app)
    {
        BindlessManager& bindless = app.resources.bindless();

        // material
        {
            CHECK(bindless.load("material", { &bindless.getBuffer("materials").buffer }));
        }

        // texture
        {
            std::vector<StringId> texIdVec = app.resources.registry().getMap<Geometry::Texture>().makeIdVec();
            std::sort(texIdVec.begin(), texIdVec.end());
            std::vector<const Geometry::Texture*> texVec = app.resources.registry().getMap<Geometry::Texture>().makePtrVec(texIdVec);
            CHECK(bindless.load("texture", texVec));
        }

        // cubemap
        {
            std::vector<StringId> cubeIdVec = app.resources.registry().getMap<RenderLoop::Cubemap>().makeIdVec();
            std::sort(cubeIdVec.begin(), cubeIdVec.end());
            std::vector<const Geometry::Texture*> texVec;
            texVec.reserve(cubeIdVec.size());
            for (StringId id : cubeIdVec)
            {
                texVec.push_back(&app.resources.get<RenderLoop::Cubemap>(id).texArray);
            }
            CHECK(bindless.load("cubemap", texVec));
        }

        // samplers
        {
            std::vector<vk::Sampler> samplerVec;
            samplerVec.reserve(app.samplers.numSamplers());
            for (u32 i = 0; i < app.samplers.numSamplers(); i++)
            {
                samplerVec.push_back(*app.samplers.sampler(i));
            }
            CHECK(bindless.load("sampler", samplerVec));
        }

        bindless.submitLoadToGpu();

        return Ok();
    }
}