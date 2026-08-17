#pragma once
#include "../utilities/common.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Resources
{
    Result<Unit, std::string> initializeResourceManager(App& app);
    Result<Unit, std::string> loadAllResources(App& app);
    Result<Unit, std::string> addBindlessResources(App& app);
    Result<Unit, std::string> handleAssetBuild(App& app);
    Result<Unit, std::string> handlePrePipelineInitialization(App& app);
    Result<Unit, std::string> handlePostLoadInitialization(App& app);
    Result<Unit, std::string> loadAndSubmitBindlessResources(App& app);
}