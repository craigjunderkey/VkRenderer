#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication { class App; }


namespace HelloTriangleApplication::Resources
{
    class TextureDescriptionManager;

    struct OfflineProcessingData
    {
        /*
            owned by resource `texture`
            ptr lifetime: owner `tasks::onBuildStarted(...)` to owner `tasks::onBuildFinished(...)`
            writing: only before `texture` resource is linked
        */
        TextureDescriptionManager* texDescs;
    };
    
    struct RuntimeProcessingData
    {
        /*
            owned by `ResourceManager`
            ptr lifetime: entire application runtime
        */
        App* app;

        /*
            owned by resource `texture`
            ptr lifetime: owner `tasks::onStartup(...)` to owner `tasks::onShutdown(...)`
            writing: only by owner
        */
        TextureDescriptionManager* texDescs;
    };
}