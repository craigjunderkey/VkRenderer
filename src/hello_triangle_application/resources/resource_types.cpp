#include "resource_types.h"

namespace HelloTriangleApplication::Resources::Types
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

    ResourceType jsonData()
    {
        return ResourceType {
            .resourceName = "json",
            .resourceDirectory = "json",
            .inFileExtensions = { ".json" },
            .outFileExtension = ".json",
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