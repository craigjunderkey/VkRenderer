#pragma once
#include "../utilities/vulkan_common.h"
#include "../utilities/common.h"
#include "../utilities/vulkan_memory_helper.h"

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::Geometry
{
    struct FatBuffer
    {
        vk::raii::Buffer buffer = nullptr;
        Utilities::DeviceMemoryBlock memory = {};
        vk::DeviceSize size = 0;
        std::unique_ptr<std::vector<u32>> families = nullptr;

        constexpr vk::raii::Buffer& operator*()
        {
            assert(this->buffer != nullptr);
            return this->buffer;
        }
        constexpr const vk::raii::Buffer& operator*() const
        {
            assert(this->buffer != nullptr);
            return this->buffer;
        }

        static Result<FatBuffer, ErrorMessage> create(
            App& app,
            vk::DeviceSize size,
            vk::BufferUsageFlags usage,
            vk::MemoryPropertyFlags properties,
            Option<std::vector<u32>> families = {}
        );
    };

    struct MappedBuffer
    {
        FatBuffer buffer = {};
        Utilities::HostMemoryMapping mapping = {};

        static Result<MappedBuffer, ErrorMessage> fromBuffer(FatBuffer&& buffer);
        static Result<MappedBuffer, ErrorMessage> fromBufferResult(Result<FatBuffer, ErrorMessage>&& bufferResult);

        FatBuffer&& unmap();

        constexpr FatBuffer& operator*() { return this->buffer; }
        constexpr const FatBuffer& operator*() const { return this->buffer; }
    };
}