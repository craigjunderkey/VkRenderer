#include "../hello_triangle_application.h"
#include "vertex_buffer.h"
#include "vertex.h"

using HelloTriangleApplication::App;
using HelloTriangleApplication::Geometry::Vertex;

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

Result<Unit, std::string> allocateMemory(App& app)
{
    vk::MemoryRequirements memRequirements = app.vertexBuffer.getMemoryRequirements();

    Result<u32, std::string> memTypeIndex = findMemoryType(
        app,
        memRequirements.memoryTypeBits,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    );

    if (!memTypeIndex)
        return Err<std::string>(memTypeIndex.error());

    vk::MemoryAllocateInfo memAllocateInfo {
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = memTypeIndex.value()
    };

    TRY_ASSIGN(
        app.vertexBufferMemory,
        castVkResult(app.device.allocateMemory(memAllocateInfo))
    );

    return Ok();
}

Result<Unit, std::string> fillBuffer(App& app, vk::BufferCreateInfo bufferInfo, const std::vector<Vertex>& vertices)
{
    app.vertexBuffer.bindMemory(*app.vertexBufferMemory, 0);

    void* data;
    TRY_ASSIGN(
        data,
        castVkResult(app.vertexBufferMemory.mapMemory(0, bufferInfo.size))
    );

    memcpy(data, vertices.data(), bufferInfo.size);

    app.vertexBufferMemory.unmapMemory();

    return Ok();
}

namespace HelloTriangleApplication::Geometry
{
    Result<Unit, std::string> createVertexBuffer(App& app)
    {
        app.vertexBufferFamilies = {
            app.graphicsQueue.familyIndex,
            app.transferQueue.familyIndex
        };

        vk::BufferCreateInfo bufferCreateInfo {
            .size = sizeof(VERTICES[0]) * VERTICES.size(),
            .usage = vk::BufferUsageFlagBits::eVertexBuffer,
            .sharingMode = vk::SharingMode::eConcurrent,
            .queueFamilyIndexCount = (u32)app.vertexBufferFamilies.size(),
            .pQueueFamilyIndices = app.vertexBufferFamilies.data()
        };

        TRY_ASSIGN(
            app.vertexBuffer,
            castVkResult(app.device.createBuffer(bufferCreateInfo))
        );

        CHECK(allocateMemory(app));
        CHECK(fillBuffer(app, bufferCreateInfo, VERTICES));

        return Ok();
    }
}