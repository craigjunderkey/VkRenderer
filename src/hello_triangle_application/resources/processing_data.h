#pragma once
#include "../utilities/common.h"
#include "../../utility/string_id_map.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Resources
{
    class TextureDescriptionManager;

    struct OfflineProcessingData
    {
        /*
            - owned by resource `texture`
            - ptr lifetime: owner `tasks::onBuildStarted(...)` to owner `tasks::onBuildFinished(...)`
            - writing: only before `texture` resource is linked
        */
        TextureDescriptionManager* texDescs;

        /*
            for each resource type that has finished processing in the current stage, `resourceCounts` contains
            the number of files outputted for this stage (i.e., exporting, compiling, linking).
            - owned by `AssetConditioningPipeline`
            - writing: only by owner
        */
        StringIdMap<u32> resourceCounts { "resource_counts" };
    };
    
    struct RuntimeProcessingData
    {
        /*
            - owned by `ResourceManager`
            - ptr lifetime: entire application runtime
        */
        App* app;

        /*
            - owned by resource `texture`
            - ptr lifetime: owner `tasks::onStartup(...)` to owner `tasks::onShutdown(...)`
            - writing: only by owner
        */
        TextureDescriptionManager* texDescs;
    };
}