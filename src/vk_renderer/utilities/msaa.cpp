#include "../vk_renderer.h"
#include "msaa.h"

using VkRenderer::Setting::AntiAliasing;

namespace VkRenderer::Utilities
{
    vk::SampleCountFlagBits getMaxUsableSampleCount(vk::raii::PhysicalDevice& device)
    {
        vk::PhysicalDeviceProperties properties = device.getProperties();

        vk::SampleCountFlags counts = properties.limits.framebufferColorSampleCounts &
            properties.limits.framebufferDepthSampleCounts;
        
        if (counts & vk::SampleCountFlagBits::e64) { return vk::SampleCountFlagBits::e64; }
        if (counts & vk::SampleCountFlagBits::e32) { return vk::SampleCountFlagBits::e32; }
        if (counts & vk::SampleCountFlagBits::e16) { return vk::SampleCountFlagBits::e16; }
        if (counts & vk::SampleCountFlagBits::e8) { return vk::SampleCountFlagBits::e8; }
        if (counts & vk::SampleCountFlagBits::e4) { return vk::SampleCountFlagBits::e4; }
        if (counts & vk::SampleCountFlagBits::e2) { return vk::SampleCountFlagBits::e2; }

        return vk::SampleCountFlagBits::e1;
    }

    bool deviceSupportsSampleCount(vk::raii::PhysicalDevice& device, vk::SampleCountFlags wish)
    {
        vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
        vk::SampleCountFlags supportedCounts = deviceProperties.limits.framebufferDepthSampleCounts
            & deviceProperties.limits.framebufferColorSampleCounts;
        
        return static_cast<bool>(wish & supportedCounts);
    }

    vk::SampleCountFlagBits aaSettingToSampleCount(AntiAliasing setting)
    {
        vk::SampleCountFlagBits desired;

        // get desired sample count from anti-aliasing setting
        switch (setting)
        {
            case AntiAliasing::None:
                desired = vk::SampleCountFlagBits::e1;
                break;
            case AntiAliasing::MSAAx2:
                desired = vk::SampleCountFlagBits::e2;
                break;
            case AntiAliasing::MSAAx4:
                desired = vk::SampleCountFlagBits::e4;
                break;
            case AntiAliasing::MSAAx8:
                desired = vk::SampleCountFlagBits::e8;
                break;
        }

        return desired;
    }

    Result<vk::SampleCountFlagBits, ErrorMessage> getSampleCount(App& app)
    {
        vk::SampleCountFlagBits desired = aaSettingToSampleCount(App::CONFIG.graphics.antiAliasing);
        if (deviceSupportsSampleCount(app.physicalDevice, desired))
        {
            return desired;
        }
        else
        {
            return ErrorMessage({ .channel = "vulkan" }, "Anti-aliasing setting not supported.");
        }
    }
}