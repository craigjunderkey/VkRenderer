cmake_minimum_required(VERSION 3.28)
project(VkRenderer)

# use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(ENABLE_CPP20_MODULE Off)

# for whatever reason the default standard library that CMake uses is wrong
# so, switch it to this one
add_compile_options("-stdlib=libc++")
add_link_options("-stdlib=libc++")

add_subdirectory(src)