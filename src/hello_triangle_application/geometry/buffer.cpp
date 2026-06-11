#include "../hello_triangle_application.h"
#include "buffer.h"

using HelloTriangleApplication::App;

Result<u32, std::string> findMemoryType(App& app, u32 typeFilter, vk::MemoryPropertyFlags properties)
{
    vk::PhysicalDeviceMemoryProperties availableTypes = app.physicalDevice.getMemoryProperties();

    for (u32 i = 0; i < availableTypes.memoryTypeCount; i++)
    {
        bool satisfiesTypeFilter = typeFilter & (1 << i);
        bool hasRequiredFeatures = availableTypes.memoryTypes[i].propertyFlags == properties;

        if (satisfiesTypeFilter && hasRequiredFeatures)
        {
            return i;
        }
    }

    return Err<std::string>("Failed to find suitable memory type.");
}

Result<vk::raii::DeviceMemory, std::string> allocateMemory(App& app, vk::raii::Buffer& buffer, vk::MemoryPropertyFlags properties)
{
    vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();

    Result<u32, std::string> memTypeIndex = findMemoryType(
        app,
        memRequirements.memoryTypeBits,
        properties
    );

    if (!memTypeIndex)
        return Err<std::string>(memTypeIndex.error());

    vk::MemoryAllocateInfo memAllocateInfo {
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = memTypeIndex.value()
    };

    return std::move(castVkResult(app.device.allocateMemory(memAllocateInfo)));
}

namespace HelloTriangleApplication::Geometry
{
    Result<FatBuffer, std::string> FatBuffer::create(
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

        TRY_ASSIGN(
            buffer.buffer,
            castVkResult(app.device.createBuffer(bufferCreateInfo))
        );
        
        TRY_ASSIGN(
            buffer.memory,
            allocateMemory(app, buffer.buffer, properties)
        );

        buffer.buffer.bindMemory(*buffer.memory, 0);

        return std::move(buffer);
    }
}