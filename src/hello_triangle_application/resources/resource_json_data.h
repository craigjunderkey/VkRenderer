#pragma once
#include "../utilities/common.h"
#include "resource_type.h"

namespace HelloTriangleApplication::Resources
{
    namespace Types
    {
        ResourceType jsonData()
        {
            return ResourceType {
                .resourceName = "json",
                .resourceDirectory = "json",
                .inFileExtensions = { ".json" },
                .outFileExtension = ".json",
                .offline = {
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
}