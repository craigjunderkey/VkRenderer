#include "../vk_renderer.h"
#include "staged_copy.h"
#include "vulkan_debug.h"

namespace VkRenderer::Utilities
{
    StagedCopy& StagedCopy::begin(void* data, u32 size)
    {
        assert(this->state_ == StagedCopyState::NotBegan);
        this->state_ = StagedCopyState::Began;

        // make the staging buffer
        auto stagingBufferCreateResult = FatBuffer::create(
            this->app_,
            size,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eDeviceLocal
        );
        if (!stagingBufferCreateResult)
        {
            this->result_ = ErrorMessage({ .channel = "vulkan" },
                "Failed to create a staging buffer for data={}, size={}.", data, size);
            return *this;
        }
        this->stagingBuffer_ = stagingBufferCreateResult.take();
        setDebugName(this->app_, **this->stagingBuffer_, "Buffer_StagedCopy");

        // map its memory
        auto mapResult = this->stagingBuffer_.memory->map();
        if (!mapResult)
        {
            this->result_ = ErrorMessage({ .channel = "vulkan" },
                "Failed to map staging buffer memory for data={}, size={}.", data, size);
            return *this;
        }
        Utilities::HostMemoryMapping mapping = mapResult.take();

        // copy data to staging buffer
        memcpy(mapping.data(), data, size);

        // create command buffer
        vk::CommandBufferAllocateInfo cmdAllocInfo {
            .commandPool = *this->app_.transferQueue.commandPool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1
        };
        auto cmdBufferResult = castVkResult(this->app_.device.allocateCommandBuffers(cmdAllocInfo));
        if (!cmdBufferResult)
        {
            this->result_ = ErrorMessage({ .channel = "vulkan" },
                "Failed to create a command buffer for data={}, size={}.", data, size);
            return *this;
        }
        this->cmdBuffer_ = std::move(cmdBufferResult.take().front());

        // begin recording
        this->cmdBuffer_.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

        return *this;
    }

    StagedCopy& StagedCopy::addDst(vk::raii::Buffer& buffer)
    {
        assert(this->state_ == StagedCopyState::Began || this->state_ == StagedCopyState::InProgress);
        this->state_ = StagedCopyState::InProgress;
        if (this->result_.isErr())
            return *this;

        this->cmdBuffer_.copyBuffer(**this->stagingBuffer_, *buffer, vk::BufferCopy { .size = this->stagingBuffer_.size });

        return *this;
    }

    StagedCopy& StagedCopy::addDst(vk::raii::Buffer& buffer, std::initializer_list<vk::BufferCopy> regions)
    {
        assert(this->state_ == StagedCopyState::Began || this->state_ == StagedCopyState::InProgress);
        this->state_ = StagedCopyState::InProgress;
        if (this->result_.isErr())
            return *this;

        this->cmdBuffer_.copyBuffer(**this->stagingBuffer_, *buffer, regions);

        return *this;
    }

    Result<Unit, ErrorMessage> StagedCopy::end()
    {
        assert(this->state_ == StagedCopyState::InProgress);
        this->state_ = StagedCopyState::Ended;
        if (this->result_.isErr())
            return std::move(this->result_);

        // perform copy
        vk::SubmitInfo submitInfo {
            .commandBufferCount = 1,
            .pCommandBuffers = &*this->cmdBuffer_
        };
        auto endResult = castVkResult(this->cmdBuffer_.end());
        CHECK(endResult);
        auto submitResult = castVkResult(this->app_.transferQueue.queue.submit(submitInfo));
        CHECK(submitResult);
        auto waitResult = castVkResult(this->app_.transferQueue.queue.waitIdle());
        CHECK(waitResult);

        return Ok();
    }
}