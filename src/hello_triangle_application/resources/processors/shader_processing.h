#pragma once
#include "../../utilities/common.h"
#include "../resource_type.h"
#include <slang-com-ptr.h>

namespace slang { class IGlobalSession; class ISession; }

namespace HelloTriangleApplication::Resources
{
    class ShaderExporter : public ResourceExporter
    {
    public:
        Result<Unit, std::string> exportResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override;
    };

    class ShaderLinker : public ResourceLinker
    {
    private:
        Slang::ComPtr<slang::IGlobalSession> globalSession_;
        Slang::ComPtr<slang::ISession> session_;

    public:
        Result<Unit, std::string> begin(OfflineProcessingData& processingData) override;
        Result<Unit, std::string> linkResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override;
        Result<Unit, std::string> end(OfflineProcessingData& processingData) override;
    };

    class ShaderLoader : public ResourceLoader
    {
    public:
        Result<Unit, std::string> loadResource(const std::filesystem::path& path, std::any resource, RuntimeProcessingData& processingData) override;
    };
}