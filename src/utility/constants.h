#include "types.h"
#include <vector>

constexpr u32 WIDTH = 1600;
constexpr u32 HEIGHT = 1200;

constexpr const char* TITLE = "HOUSE OF HENRY";

const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

#ifndef NDEBUG
constexpr bool VALIDATION_LAYERS_ENABLED = false;
#else
constexpr bool VALIDATION_LAYERS_ENABLED = true;
#endif