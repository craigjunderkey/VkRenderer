#include "resource_types.h"
#include "processors/resource_loaders.h"
#include "processors/mesh_processing.h"
#include "processors/image_processing.h"
#include "processors/material_processing.h"
#include "processors/texture_processing.h"
#include "processors/shader_processing.h"
#include "processors/cubemap_processing.h"

namespace HelloTriangleApplication::Resources::Types
{
    ResourceType textData()
    {
        static TextDataLoader loader = TextDataLoader();

        return ResourceType {
            .resourceName = "text",
            .resourceDirectory = "text",
            .inFileExtensions = { ".txt" },
            .offline {},
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
            .offline {},
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
            .offline {},
            .runtime {
                .loader = &loader,
                .unloader = &unloader
            }
        };
    }

    ResourceType mesh()
    {
        static MeshExporter exporter = MeshExporter();
        static MeshLoader loader = MeshLoader();
        static MeshInitializer initializer = MeshInitializer();

        return ResourceType {
            .resourceName = "mesh",
            .resourceDirectory = "meshes",
            .inFileExtensions = { ".obj" },
            .offline {
                .exporter = &exporter
            },
            .runtime {
                .loader = &loader,
                .unloader = {},
                .postLoadInitializer = &initializer
            }
        };
    }

    ResourceType material()
    {
        static MaterialExporter exporter = MaterialExporter();
        static MaterialLoader loader = MaterialLoader();
        static MaterialInitializer initializer = MaterialInitializer();

        return ResourceType {
            .resourceName = "material",
            .resourceDirectory = "materials",
            .inFileExtensions = { ".mats" },
            .offline {
                .exporter = &exporter
            },
            .runtime {
                .loader = &loader,
                .unloader = {},
                .postLoadInitializer = &initializer
            }
        };
    }

    ResourceType texture()
    {
        static TextureExporter exporter = TextureExporter();
        static TextureLinker linker = TextureLinker();
        static OfflineTextureTasks offlineTasks = OfflineTextureTasks();
        static TextureLoader loader = TextureLoader();
        static TextureInitializer initializer = TextureInitializer();
        static RuntimeTextureTasks runtimeTasks = RuntimeTextureTasks();

        return ResourceType {
            .resourceName = "texture",
            .resourceDirectory = "textures",
            .inFileExtensions = { ".png", ".jpg" },
            .offline {
                .exporter = &exporter,
                .compiler = {},
                .linker = &linker,
                .tasks = &offlineTasks
            },
            .runtime {
                .loader = &loader,
                .unloader = {},
                .postLoadInitializer = &initializer,
                .tasks = &runtimeTasks
            }
        };
    }
    
    ResourceType shader()
    {
        static ShaderExporter exporter = ShaderExporter();
        static ShaderLinker linker = ShaderLinker();
        static ShaderLoader loader = ShaderLoader();

        return ResourceType {
            .resourceName = "shader",
            .resourceDirectory = "shaders",
            .inFileExtensions = { ".slang" },
            .offline {
                .exporter = &exporter,
                .compiler = {},
                .linker = &linker
            },
            .runtime {
                .loader = &loader
            }
        };
    }

    ResourceType cubemap()
    {
        static CubemapExporter exporter = CubemapExporter();
        static CubemapLoader loader = CubemapLoader();
        static CubemapInitializer initializer = CubemapInitializer();

        return ResourceType {
            .resourceName = "cubemap",
            .resourceDirectory = "cubemaps",
            .inFileExtensions = { ".cubemap" },
            .offline {
                .exporter = &exporter
            },
            .runtime {
                .loader = &loader,
                .postLoadInitializer = &initializer
            }
        };
    }
}