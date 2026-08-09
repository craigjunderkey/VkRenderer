#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication { class App; }


namespace HelloTriangleApplication::Resources
{
    class TextureDescriptionManager;

    struct OfflineProcessingData
    {
        TextureDescriptionManager* texDescs;
    };
    
    struct RuntimeProcessingData
    {
        App* app;
        TextureDescriptionManager* texDescs;
    };
}