#include "shader_processing.h"
#include <iostream>
#include <fstream>
#include "shader.h"
#include "../render_loop/sampler_name.h"

#include <slang.h>

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> castSlangResult(SlangResult result)
    {
        if (SLANG_SUCCEEDED(result))
        {
            return Ok();
        }
        else
        {
            return Err<std::string>("Slang error: " + std::to_string(result) +" .");
        }
    }

    #define PRINT_DIAGNOSTICS(x) \
        if (x) \
            std::cout << "Slang diagnostics: " << x->getBufferPointer() << std::endl;
    
    Result<Unit, std::string> ShaderExporter::exportResource(const std::filesystem::path& srcPath, OfflineProcessingData& processingData)
    {
        std::filesystem::path dstDir = getExportDirectory("shaders") / "src";
        std::filesystem::create_directory(dstDir);
        std::filesystem::path dstPath = std::move(dstDir) / srcPath.filename();
        std::ifstream src(srcPath);
        std::ofstream dst(dstPath);
        dst << src.rdbuf();
        return Ok();
    }

    Result<Unit, std::string> ShaderLinker::begin(OfflineProcessingData& processingData)
    {
        // create global session
        SlangResult globalResult = slang::createGlobalSession(this->globalSession_.writeRef());
        CHECK(castSlangResult(globalResult));

        // configure target
        std::array<slang::CompilerOptionEntry, 3> compilerOptions {
            slang::CompilerOptionEntry {
                .name = slang::CompilerOptionName::EmitSpirvDirectly,
                .value = { .intValue0 = 1 }
            },
            slang::CompilerOptionEntry {
                .name = slang::CompilerOptionName::VulkanUseEntryPointName,
                .value = { .intValue0 = 1 }
            },
            slang::CompilerOptionEntry {
                .name = slang::CompilerOptionName::GenerateWholeProgram,
                .value = { .intValue0 = 1 }
            }
        };

        slang::TargetDesc target {
            .format = SlangCompileTarget::SLANG_SPIRV,
            .profile = this->globalSession_->findProfile("spirv_1_4"),
            .compilerOptionEntries = compilerOptions.data(),
            .compilerOptionEntryCount = compilerOptions.size()
        };

        // configure macros
        auto texCountStr = std::to_string(processingData.resourceCounts.get("texture"));
        auto matCountStr = std::to_string(processingData.resourceCounts.get("material"));
        auto samplerCountStr = std::to_string(RenderLoop::SamplerName::None);
        std::array<slang::PreprocessorMacroDesc, 3> macros {
            slang::PreprocessorMacroDesc {
                .name = "NUM_TEXTURES",
                .value = texCountStr.data()
            },
            slang::PreprocessorMacroDesc {
                .name = "NUM_MATERIALS",
                .value = matCountStr.data()
            },
            slang::PreprocessorMacroDesc {
                .name = "NUM_SAMPLERS",
                .value = samplerCountStr.data()
            }
        };
        
        // configure session
        std::filesystem::path searchPath = getResourceDirectory("shaders") / "src";
        const char* searchPathCStr = searchPath.c_str();
        slang::SessionDesc desc {
            .targets = &target,
            .targetCount = 1,

            // default matrix layout is actually row-major; for some reason,
            // setting it to column-major with this enum sets it to row-major
            // strange...
            .defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR,
            
            .searchPaths = &searchPathCStr,
            .searchPathCount = 1,
            .preprocessorMacros = macros.data(),
            .preprocessorMacroCount = macros.size()
        };

        // create session
        SlangResult sessionResult = this->globalSession_->createSession(desc, this->session_.writeRef());
        CHECK(castSlangResult(sessionResult));

        return Ok();
    }

    /*
    .shader format:
        u32 numEntryPoints
        u32 codeLength
        for 0..numEntryPoints
            ShaderStage stage
            char[] funcName
        char[] code
    */
    Result<Unit, std::string> ShaderLinker::linkResource(const std::filesystem::path& path, OfflineProcessingData& processingData)
    {
        // load shader code
        std::filesystem::path moduleName = path.stem();
        const char* moduleNameCStr = moduleName.c_str();
        Slang::ComPtr<slang::IBlob> diagnostics;
        Slang::ComPtr<slang::IModule> shaderModule(this->session_->loadModule(moduleNameCStr, diagnostics.writeRef()));
        if (diagnostics)
            return Err<std::string>(reinterpret_cast<const char*>(diagnostics->getBufferPointer()));
        
        // get all entry points
        std::vector<Slang::ComPtr<slang::IEntryPoint>> slangEntryPoints;
        for (u32 i = 0; i < shaderModule->getDefinedEntryPointCount(); i++)
        {
            Slang::ComPtr<slang::IEntryPoint> entryPoint;
            SlangResult result = shaderModule->getDefinedEntryPoint(i, entryPoint.writeRef());
            CHECK(castSlangResult(result));
            slangEntryPoints.emplace_back(std::move(entryPoint));
        }

        // store entry point reflection data
        std::vector<ShaderEntryPoint> entryPointReflections;
        for (u32 i = 0; i < slangEntryPoints.size(); i++)
        {
            slang::FunctionReflection* reflection = slangEntryPoints[i]->getFunctionReflection();

            // store entry point name
            ShaderEntryPoint entryPoint {
                .functionName = reflection->getName()
            };

            // get shader stage from attribute
            Option<ShaderStage> stage;
            slang::Attribute* shaderAttribute = reflection->findAttributeByName(this->globalSession_, "shader");
            if (shaderAttribute)
            {
                size_t strSize;
                const char* type = shaderAttribute->getArgumentValueString(0, &strSize);
                stage = Shader::stringToStage(type);

                if (!stage.has_value())
                {
                    return Err<std::string>("Found an entry point in " + moduleName.string() + " (" + entryPoint.functionName
                        + "), but the argument for [shader(...)] is not valid.");
                }
            }

            // ensure each entry point has a [shader(...)] attribute
            if (!stage.has_value())
            {
                return Err<std::string>("Found an entry point in " + moduleName.string() + " (" + entryPoint.functionName
                    + "), but it isn't marked with a [shader(...)] attribute.");
            }
            else
            {
                entryPoint.stage = stage.value();
            }

            entryPointReflections.emplace_back(std::move(entryPoint));
        }

        // return early if no [shader(...)] entry points; we can't compile something without an entry point
        if (entryPointReflections.empty())
            return Ok();
        
        // create composite component type
        std::vector<slang::IComponentType*> components = { shaderModule };
        for (u32 i = 0; i < slangEntryPoints.size(); i++)
        {
            components.push_back(slangEntryPoints[i]);
        }
        Slang::ComPtr<slang::IComponentType> program;
        SlangResult compositeResult = this->session_->createCompositeComponentType(
            components.data(), components.size(), program.writeRef(), diagnostics.writeRef());
        PRINT_DIAGNOSTICS(diagnostics);
        CHECK(castSlangResult(compositeResult));

        // link the program
        Slang::ComPtr<slang::IComponentType> linkedProgram;
        SlangResult linkResult = program->link(linkedProgram.writeRef(), diagnostics.writeRef());
        PRINT_DIAGNOSTICS(diagnostics);
        CHECK(castSlangResult(linkResult));

        // extract program kernel code
        Slang::ComPtr<slang::IBlob> kernelBlob;
        SlangResult kernelResult = linkedProgram->getTargetCode(0, kernelBlob.writeRef(), diagnostics.writeRef());
        PRINT_DIAGNOSTICS(diagnostics);
        CHECK(castSlangResult(kernelResult));

        // write to file
        std::filesystem::path dstPath = getResourceDirectory("shaders") / path.stem().replace_extension(".shader");
        std::ofstream dst(dstPath, std::ios::binary);
       
        dst << static_cast<u32>(entryPointReflections.size()) << " " << kernelBlob->getBufferSize() << "\n";
        for (const ShaderEntryPoint& entryPoint : entryPointReflections)
        {
            dst << static_cast<u32>(entryPoint.stage) << " " << entryPoint.functionName.c_str() << "\n";
        }
        dst.write(reinterpret_cast<const char*>(kernelBlob->getBufferPointer()), kernelBlob->getBufferSize());

        return Ok();
    }

    Result<Unit, std::string> ShaderLinker::end(OfflineProcessingData& processingData)
    {
        // destroy session
        this->session_.setNull();
        this->globalSession_.setNull();

        // delete src
        std::filesystem::path src = getResourceDirectory("shaders") / "src";
        std::filesystem::remove_all(src);
        std::filesystem::remove(src);

        return Ok();
    }

    Result<Unit, std::string> ShaderLoader::loadResource(const std::filesystem::path& path, std::any resource, RuntimeProcessingData& processingData)
    {
        Shader* shader = std::any_cast<Shader*>(resource);
        shader->name = pathToName(path);

        std::ifstream in(path, std::ios::binary);
        
        // read lengths
        u32 numEntryPoints;
        u32 codeLength;
        in >> numEntryPoints >> codeLength;
        
        // read numEntryPoints entry points
        shader->entryPoints.reserve(numEntryPoints);
        for (u32 i = 0; i < numEntryPoints; i++)
        {
            ShaderEntryPoint entryPoint;
            
            u32 stageNum;
            in >> stageNum;
            entryPoint.stage = static_cast<ShaderStage>(stageNum);
            in >> entryPoint.functionName;

            shader->entryPoints.emplace_back(std::move(entryPoint));
        }
        in.ignore();

        // read the compiled shader code
        shader->code.resize(codeLength);
        in.read(shader->code.data(), codeLength);

        // check that we've reached the end of the file
        in.ignore();
        if (!in.eof())
            return Err<std::string>("Read all shader information in " + path.string() + " but didn't reach end-of-file.\n");

        return Ok();
    }
}