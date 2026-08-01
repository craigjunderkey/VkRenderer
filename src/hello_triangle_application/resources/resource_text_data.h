#pragma once
#include "../utilities/common.h"
#include "resource_type.h"
#include <vector>
#include <string>

namespace HelloTriangleApplication::Resources
{
    namespace Types
    {
        ResourceType textData()
        {
            return ResourceType {
                .resourceName = "text",
                .resourceDirectory = "text",
                .inFileExtensions = { ".txt" },
                .outFileExtension = ".txt",
                .offline {
                    .exporter = {},
                    .compiler = {},
                    .linker = {}
                },
                .runtime {
                    .loader = nullptr,
                    .unloader = {}
                }
            };
        }
    }
    
    struct TextData
    {
        std::vector<std::string> data;
    };
}