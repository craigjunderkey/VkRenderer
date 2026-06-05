# disable Vulkan constructors so we can use initializer lists
add_definitions( -DVULKAN_HPP_NO_CONSTRUCTORS )

# disable Vulkan's use of exceptions so we can use std::expected for error-handling
add_definitions( -DVULKAN_HPP_NO_EXCEPTIONS )

# setup Vulkan
include("../CMake/VulkanSetup.cmake")
