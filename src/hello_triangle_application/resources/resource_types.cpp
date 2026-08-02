#include "resource_types.h"
#include "resource_loaders.h"
#include "resource_unloaders.h"

namespace HelloTriangleApplication::Resources::Types
{
    ResourceType textData()
    {
        static TextDataLoader loader = TextDataLoader();

        return ResourceType {
            .resourceName = "text",
            .resourceDirectory = "text",
            .inFileExtensions = { ".txt" },
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

    ResourceType imageData()
    {
        static ImageDataLoader loader = ImageDataLoader();
        static ImageDataUnloader unloader = ImageDataUnloader();

        return ResourceType {
            .resourceName = "image",
            .resourceDirectory = "images",
            .inFileExtensions = { ".png", ".jpg" },
            .offline {
                .exporter = {},
                .compiler = {},
                .linker = {}
            },
            .runtime {
                .loader = &loader,
                .unloader = &unloader
            }
        };
    }
}