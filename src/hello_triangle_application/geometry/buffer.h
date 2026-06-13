#pragma once
#include "../utilities/vulkan_common.h"
#include "../utilities/common.h"

namespace HelloTriangleApplication
{
    class App;
}

namespace HelloTriangleApplication::Geometry
{
    struct MappedBuffer;
    
    struct FatBuffer
    {
        vk::raii::Buffer buffer = nullptr;
        vk::raii::DeviceMemory memory = nullptr;
        vk::DeviceSize size = 0;
        std::unique_ptr<std::vector<u32>> families = nullptr;

        vk::raii::Buffer& operator*() { return this->buffer; }

        static Result<FatBuffer, std::string> create(
            App& app,
            vk::DeviceSize size,
            vk::BufferUsageFlags usage,
            vk::MemoryPropertyFlags properties,
            Option<std::vector<u32>> families = None()
        );

        Result<MappedBuffer, std::string> makeMapped();
    };

    struct MappedBuffer
    {
        FatBuffer buffer = {};
        void* data = nullptr;

        FatBuffer& operator*() { return this->buffer; }

        static Result<MappedBuffer, std::string> fromFatBufferResult(Result<FatBuffer, std::string>&& fatBufferResult);
    };

    Result<vk::raii::DeviceMemory, std::string> allocateMemory(App& app, vk::raii::Buffer& buffer, vk::MemoryPropertyFlags properties);
    Result<u32, std::string> findMemoryType(App& app, u32 typeFilter, vk::MemoryPropertyFlags properties);
}