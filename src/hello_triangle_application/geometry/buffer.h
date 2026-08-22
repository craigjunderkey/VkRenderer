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
        const vk::raii::Buffer& operator*() const { return this->buffer; }

        static Result<FatBuffer, ErrorMessage> create(
            App& app,
            vk::DeviceSize size,
            vk::BufferUsageFlags usage,
            vk::MemoryPropertyFlags properties,
            Option<std::vector<u32>> families = {}
        );

        Result<MappedBuffer, ErrorMessage> makeMapped();
    };

    struct MappedBuffer
    {
        FatBuffer buffer = {};
        void* data = nullptr;

        FatBuffer& operator*() { return this->buffer; }
        FatBuffer&& unmap();

        static Result<MappedBuffer, ErrorMessage> fromFatBufferResult(Result<FatBuffer, ErrorMessage>&& fatBufferResult);
    };

    Result<vk::raii::DeviceMemory, ErrorMessage> allocateMemory(App& app, vk::raii::Buffer& buffer, vk::MemoryPropertyFlags properties);
    Result<u32, ErrorMessage> findMemoryType(App& app, u32 typeFilter, vk::MemoryPropertyFlags properties);
}