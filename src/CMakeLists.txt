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
if (LINUX)
    set(${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/..")
    include(../CMake/slang/slangConfig.cmake)
    include(../CMake/slang/slangTargets.cmake)
    set(SLANG_INCLUDEDIR "$ENV{VULKAN_SDK}/include/slang")
    target_include_directories(${PROJECT_NAME} PRIVATE ${SLANG_INCLUDEDIR})
endif()

find_package(slang REQUIRED)
target_link_libraries(${PROJECT_NAME} slang::slang)

# precompile heavy headers
target_precompile_headers(
    ${PROJECT_NAME}
    PRIVATE
        <unordered_map>
        <vector>
        <iostream>
        <expected>
        <cstdlib>
        <chrono>
        <nlohmann/json.hpp>
        <filesystem>
        <fstream>
        <memory>
        <concepts>
        <vulkan/vulkan_raii.hpp>
        <vulkan/vulkan_to_string.hpp>
        <slang.h>
)

# copy resources
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/../assets"
    "${CMAKE_CURRENT_BINARY_DIR}/../${PROJECT_NAME}/assets"
)
