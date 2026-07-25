# disable Vulkan constructors so we can use initializer lists
add_definitions( -DVULKAN_HPP_NO_CONSTRUCTORS )

# disable Vulkan's use of exceptions so we can use std::expected for error-handling
add_definitions( -DVULKAN_HPP_NO_EXCEPTIONS )

# setup Vulkan
include("../CMake/VulkanSetup.cmake")

# include json library
find_package(nlohmann_json REQUIRED)
set(JSON_INCLUDEDIR ${nlohmann_json_INCLUDE_DIRS})
target_include_directories (${TEMPLATE_NAME} PRIVATE ${JSON_INCLUDEDIR})

# compile shaders
set(SHADER_SLANG_SOURCES "shader" "light_debug")
set(IN_SHADERS_DIR ${CMAKE_CURRENT_LIST_DIR}/shaders)
set(OUT_SHADERS_DIR ${CMAKE_CURRENT_LIST_DIR}/../shaders) # this sucks
add_slang_shader_target(SHADER_TARGETS SOURCES ${SHADER_SLANG_SOURCES})
add_dependencies(${TEMPLATE_NAME} ${SHADER_TARGETS})