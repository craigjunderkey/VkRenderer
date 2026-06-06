#pragma once

#include "types.h"
#include <vector>

constexpr u32 WIDTH = 1600;
constexpr u32 HEIGHT = 1200;

constexpr const char* TITLE = "HOUSE OF HENRY";

const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> REQUIRED_DEVICE_EXTENSIONS = {
    vk::KHRSwapchainExtensionName
};

#ifndef NDEBUG
constexpr bool VALIDATION_LAYERS_ENABLED = true;
#else
constexpr bool VALIDATION_LAYERS_ENABLED = false;
#endif