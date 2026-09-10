#pragma once
#include "common.h"
#include "vulkan_common.h"

namespace VkRenderer { class App; }

namespace VkRenderer::Utilities
{
    vk::SampleCountFlagBits getMaxUsableSampleCount(vk::raii::PhysicalDevice& device);
    Result<vk::SampleCountFlagBits, ErrorMessage> getSampleCount(App& app);
    bool deviceSupportsSampleCount(vk::raii::PhysicalDevice& device, vk::SampleCountFlags wish);
    vk::SampleCountFlagBits aaSettingToSampleCount(VkRenderer::Setting::AntiAliasing setting);
}