#include "../hello_triangle_application.h"
#include "buffer.h"

using HelloTriangleApplication::App;

namespace HelloTriangleApplication::Geometry
{    
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
        
        vk::MemoryRequirements memRequirements = buffer.buffer.getMemoryRequirements();
        auto allocMemoryResult = app.memoryHelper.allocate(memRequirements.size, properties, memRequirements);
        CHECK_KEEP(allocMemoryResult);
        buffer.memory = allocMemoryResult.take();

        buffer.buffer.bindMemory(*buffer.memory, 0);

        return std::move(buffer);
    }

    Result<MappedBuffer, ErrorMessage> MappedBuffer::fromBuffer(FatBuffer&& buffer)
    {
        auto mappingResult = buffer.memory.map();
        CHECK_KEEP(mappingResult);
        return MappedBuffer {
            .buffer = std::move(buffer),
            .mapping = mappingResult.take()
        };
    }
    
    Result<MappedBuffer, ErrorMessage> MappedBuffer::fromBufferResult(Result<FatBuffer, ErrorMessage>&& bufferResult)
    {
        CHECK_KEEP(bufferResult);
        return std::move(MappedBuffer::fromBuffer(bufferResult.take()));
    }

    FatBuffer&& MappedBuffer::unmap()
    {
        this->mapping = {};
        return std::move(this->buffer);
    }
}