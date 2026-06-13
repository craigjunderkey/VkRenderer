#pragma once

#include "types.h"
#include <vector>

constexpr u32 WIDTH = 800;
constexpr u32 HEIGHT = 600;

constexpr const char* TITLE = "HOUSE OF HENRY";

const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> REQUIRED_DEVICE_EXTENSIONS = {
    "VK_KHR_swapchain"
};

constexpr const char* SHADER_PATH = "shaders/shader.spv";
constexpr const char* TEXTURE_PATH = "src/textures/rcoulter.jpg";

#ifndef NDEBUG
constexpr bool VALIDATION_LAYERS_ENABLED = true;
#else
constexpr bool VALIDATION_LAYERS_ENABLED = false;
#endif

constexpr std::array<f32, 4> CLEAR_COLOR = {0.0f, 0.0f, 0.0f, 1.0f};

constexpr u32 MAX_FRAMES_IN_FLIGHT = 2;