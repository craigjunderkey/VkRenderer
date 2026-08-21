# disable Vulkan constructors so we can use initializer lists
add_definitions( -DVULKAN_HPP_NO_CONSTRUCTORS )

# disable Vulkan's use of exceptions so we can use std::expected for error-handling
add_definitions( -DVULKAN_HPP_NO_EXCEPTIONS )

# setup Vulkan
include("../CMake/VulkanSetup.cmake")

# include json library
find_package(nlohmann_json REQUIRED)
set(JSON_INCLUDEDIR ${nlohmann_json_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${JSON_INCLUDEDIR})

# include slang compilation API
set(${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/..")
include(../CMake/slang/slangConfig.cmake)
find_package(slang REQUIRED)
include(../CMake/slang/slangTargets.cmake)
target_link_libraries(${PROJECT_NAME} slang::slang)
set(SLANG_INCLUDEDIR "$ENV{VULKAN_SDK}/include/slang")
target_include_directories(${PROJECT_NAME} PRIVATE ${SLANG_INCLUDEDIR})

# precompile heavy headers
target_precompile_headers(
    ${PROJECT_NAME}
    PRIVATE
        <unordered_map>
        <vector>
        <iostream>
        <expected>
        <cstdlib>
        <nlohmann/json.hpp>
        <slang.h>
        <vulkan/vulkan_raii.hpp>
)
