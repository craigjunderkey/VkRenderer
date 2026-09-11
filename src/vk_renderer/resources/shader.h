#pragma once
#include "../utilities/common.h"

namespace VkRenderer::Resources
{
    constexpr u32 NUM_SHADER_STAGES = 7;

    enum class ShaderStage
    {
        Vertex,
        Fragment,
        Compute,
        Geometry,
        Hull,
        Domain,
        RayGeneration
    };

    struct ShaderEntryPoint
    {
        ShaderStage stage;
        std::string functionName;
    };

    struct Shader
    {
        StringId name;
        std::vector<ShaderEntryPoint> entryPoints;
        std::vector<char> code;

        static const char* stageToString(ShaderStage stage)
        {
            switch (stage)
            {
                case ShaderStage::Vertex:
                    return "vertex";
                case ShaderStage::Fragment:
                    return "fragment";
                case ShaderStage::Compute:
                    return "compute";
                case ShaderStage::Geometry:
                    return "geometry";
                case ShaderStage::Hull:
                    return "hull";
                case ShaderStage::Domain:
                    return "domain";
                case ShaderStage::RayGeneration:
                    return "raygeneration";
            }
        }

        static Option<ShaderStage> stringToStage(const char* stage)
        {
            for (u32 i = 0; i < NUM_SHADER_STAGES; i++)
            {
                const char* currentName = stageToString(static_cast<ShaderStage>(i));
                if (std::strcmp(stage, currentName) == 0)
                {
                    return static_cast<ShaderStage>(i);
                }
            }
            return {};
        }

        ShaderEntryPoint& getEntryPoint(ShaderStage stage)
        {
            for (u32 i = 0; i < entryPoints.size(); i++)
            {
                if (entryPoints[i].stage == stage)
                    return entryPoints[i];
            }

            panic("Failed to find an entry point in shader " + std::string(this->name) + " with stage=" + Shader::stageToString(stage) + ".");
            return entryPoints[-1]; // just here to disable a warning
        }
    };
}