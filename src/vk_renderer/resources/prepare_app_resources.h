#pragma once
#include "../utilities/common.h"

namespace VkRenderer { class App; }

namespace VkRenderer::Resources
{
    Result<Unit, ErrorMessage> initializeResourceManager(App& app);
    Result<Unit, ErrorMessage> loadAllResources(App& app);
    Result<Unit, ErrorMessage> addBindlessResources(App& app);
    Result<Unit, ErrorMessage> handleAssetBuild(App& app);
    Result<Unit, ErrorMessage> handlePrePipelineInitialization(App& app);
    Result<Unit, ErrorMessage> handlePostLoadInitialization(App& app);
    Result<Unit, ErrorMessage> loadAndSubmitBindlessResources(App& app);
}