#include "resource_types.h"
#include "resource_loaders.h"

namespace HelloTriangleApplication::Resources::Types
{
    ResourceType textData()
    {
        static TextDataLoader loader = TextDataLoader();

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
                .loader = &loader,
                .unloader = {}
            }
        };
    }

    ResourceType jsonData()
    {
        static JsonDataLoader loader = JsonDataLoader();

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
                .loader = &loader,
                .unloader = {}
            }
        };
    }
}