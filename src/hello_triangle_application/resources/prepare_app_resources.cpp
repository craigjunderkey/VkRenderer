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
        
        // make GPU material manager
        auto gpuMatResult = GpuMaterialManager::create(
            app,
            app.resources.registry().getMap<RenderLoop::Material>().size(),
            app.resources.registry().getMap<Texture>().size(),
            app.samplers.numSamplers()
        );
        CHECK_KEEP(gpuMatResult);
        app.gpuMaterials = gpuMatResult.take();

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

    Result<Unit, std::string> loadBindlessResources(App& app)
    {
        BindlessManager& bindless = app.resources.bindless();

        // material
        {
            // get ptr vec
            std::vector<StringId> matIdVec = app.resources.registry().getMap<RenderLoop::Material>().makeIdVec();
            std::sort(matIdVec.begin(), matIdVec.end());
            std::vector<const RenderLoop::Material*> matVec = app.resources.registry().getMap<RenderLoop::Material>().makePtrVec(matIdVec);

            // create material uniform data
            std::vector<RenderLoop::MaterialUniformData> matData;
            u32 numMaterials = bindless.getBindlessResource("material").virtualCount;
            matData.reserve(numMaterials);
            for (u32 i = 0; i < numMaterials; i++)
            {
                const RenderLoop::Material* mat = matVec[i];
                matData.push_back(RenderLoop::MaterialUniformData {
                    .diffMap = app.resources.bindless().getNullableIndex("texture", mat->diffuseTexture).unwrap(),
                    .specMap = app.resources.bindless().getNullableIndex("texture", mat->specularTexture).unwrap(),
                    .samplerId = static_cast<u32>(mat->sampler),
                    .shininess = mat->shininess,
                    .isUnlit = mat->isUnlit,
                    .texOffset = mat->texOffset,
                    .texScale = mat->texScale,
                    .diffColor = mat->diffuseColor,
                    .specColor = mat->specularColor
                });
            }

            // buffer the material uniform data
            Geometry::MappedBuffer& matBuffer = bindless.getBuffer("materials");
            memcpy(matBuffer.data, matData.data(), matBuffer.buffer.size);

            CHECK(bindless.load("material", { &matBuffer.buffer }));
        }

        // texture
        {
            std::vector<StringId> texIdVec = app.resources.registry().getMap<Geometry::Texture>().makeIdVec();
            std::sort(texIdVec.begin(), texIdVec.end());
            std::vector<const Geometry::Texture*> texVec = app.resources.registry().getMap<Geometry::Texture>().makePtrVec(texIdVec);
            CHECK(bindless.load("texture", texVec));
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