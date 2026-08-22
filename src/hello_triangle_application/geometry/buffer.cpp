#include "../hello_triangle_application.h"
#include "buffer.h"

using HelloTriangleApplication::App;

namespace HelloTriangleApplication::Geometry
{
    Result<u32, ErrorMessage> findMemoryType(App& app, u32 typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties availableTypes = app.physicalDevice.getMemoryProperties();

        for (u32 i = 0; i < availableTypes.memoryTypeCount; i++)
        {
            bool satisfiesTypeFilter = typeFilter & (1 << i);
            bool hasRequiredFeatures = (availableTypes.memoryTypes[i].propertyFlags & properties) == properties;

            if (satisfiesTypeFilter && hasRequiredFeatures)
            {
                return i;
            }
        }

        return ErrorMessage({ .channel = "vulkan" }, "Failed to find suitable memory type.");
    }

    Result<vk::raii::DeviceMemory, ErrorMessage> allocateMemory(App& app, vk::raii::Buffer& buffer, vk::MemoryPropertyFlags properties)
    {
        vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();

        Result<u32, ErrorMessage> memTypeIndex = findMemoryType(
            app,
            memRequirements.memoryTypeBits,
            properties
        );

        CHECK_KEEP(memTypeIndex);

        vk::MemoryAllocateInfo memAllocateInfo {
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = memTypeIndex.take()
        };

        return std::move(castVkResult(app.device.allocateMemory(memAllocateInfo)));
    }
    
    Result<FatBuffer, ErrorMessage> FatBuffer::create(
        App& app,
        vk::DeviceSize size,
        vk::BufferUsageFlags usage,
        vk::MemoryPropertyFlags properties,
        Option<std::vector<u32>> families)
    {
        FatBuffer buffer = FatBuffer { .size = size };
        
        vk::BufferCreateInfo bufferCreateInfo {
            .size = size,
            .usage = usage,
            .sharingMode = vk::SharingMode::eExclusive
        };

        if (families)
        {
            buffer.families = std::make_unique<std::vector<u32>>(std::move(families).value());
            bufferCreateInfo.queueFamilyIndexCount = buffer.families->size();
            bufferCreateInfo.pQueueFamilyIndices = buffer.families->data();
            bufferCreateInfo.sharingMode = vk::SharingMode::eConcurrent;
        }

        auto createBufferResult = castVkResult(app.device.createBuffer(bufferCreateInfo));
        CHECK_KEEP(createBufferResult);
        buffer.buffer = createBufferResult.take();
        
        auto allocMemoryResult = allocateMemory(app, buffer.buffer, properties);
        CHECK_KEEP(allocMemoryResult);
        buffer.memory = allocMemoryResult.take();

        buffer.buffer.bindMemory(*buffer.memory, 0);

        return std::move(buffer);
    }

    Result<MappedBuffer, ErrorMessage> FatBuffer::makeMapped()
    {
        auto mapMemoryResult = castVkResult(this->memory.mapMemory(0, this->size));
        CHECK_KEEP(mapMemoryResult);
        
        return MappedBuffer {
            .buffer = std::move(*this),
            .data = mapMemoryResult.take()
        };
    }

    Result<MappedBuffer, ErrorMessage> MappedBuffer::fromFatBufferResult(Result<FatBuffer, ErrorMessage>&& fatBufferResult)
    {
        CHECK_KEEP(fatBufferResult);
        return fatBufferResult.take().makeMapped();
    }

    FatBuffer&& MappedBuffer::unmap()
    {
        this->data = nullptr;
        return std::move(this->buffer);
    }
}