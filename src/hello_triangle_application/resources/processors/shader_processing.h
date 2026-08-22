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
        Result<Unit, ErrorMessage> exportResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override;
    };

    class ShaderLinker : public ResourceLinker
    {
    private:
        Slang::ComPtr<slang::IGlobalSession> globalSession_;
        Slang::ComPtr<slang::ISession> session_;

    public:
        Result<Unit, ErrorMessage> begin(OfflineProcessingData& processingData) override;
        Result<Unit, ErrorMessage> linkResource(const std::filesystem::path& path, OfflineProcessingData& processingData) override;
        Result<Unit, ErrorMessage> end(OfflineProcessingData& processingData) override;
    };

    class ShaderLoader : public ResourceLoader
    {
    public:
        Result<Unit, ErrorMessage> loadResource(const std::filesystem::path& path, std::any resource, RuntimeProcessingData& processingData) override;
    };
}