#include "../geometry/buffer.h"
#include <initializer_list>

namespace VkRenderer { class App; }

namespace VkRenderer::Utilities
{
    enum StagedCopyState
    {
        NotBegan,
        Began,
        InProgress,
        Ended
    };

    /// @brief Represents a one-off copy to a set of (GPU) buffers using a staging buffer.
    /// @warning `StagedCopy` creates a staging buffer and a command buffer; it also waits
    /// after calling `end()`. So, don't use this class if you're looking for performance.
    /// @warning No errors are returned until `end()` is called. Any other calls will result in no-ops.
    class StagedCopy
    {
    private:
        App& app_;
        Geometry::FatBuffer stagingBuffer_ = {};
        vk::raii::CommandBuffer cmdBuffer_ = nullptr;
        Result<Unit, ErrorMessage> result_ = Ok();
        StagedCopyState state_ = StagedCopyState::NotBegan;

    public:
        /// @warning Uses `app.transferQueue` to allocate and submit copy commands. 
        StagedCopy(App& app) : app_(app) {}

        /// @brief Allocates a staging buffer and allocates given data to it.
        // Creates a command buffer and begins recording.
        /// @pre is in `NotBegan` state
        /// @post is in `Began` state
        /// @param data the source data to copy
        /// @param size the size of the data
        StagedCopy& begin(void* data, u32 size);

        /// @brief Adds buffer as a copy destination. Uses a default region of offset=0 and size=|data|.
        /// @pre buffer is at least as large as the staging buffer
        /// @pre is in `Began` or `InProgress` state
        /// @post is in `InProgress` state
        /// @param dstBuffer the destination buffer
        StagedCopy& addDst(vk::raii::Buffer& dstBuffer);
        
        /// @brief Adds buffer as a copy destination using the given regions.
        /// @pre is in `Began` or `InProgress` state
        /// @post is in `InProgress` state
        /// @param dstBuffer the destination buffer
        /// @param regions the region of the source and destination buffers
        StagedCopy& addDst(vk::raii::Buffer& dstBuffer, std::initializer_list<vk::BufferCopy> regions);

        /// @brief Submits the copy command buffer and waits until the copy is completed.
        /// @pre is in `InProgress` state
        /// @post is in `Ended` state
        /// @return the result of the entire copy process (i.e., including earlier operations)
        /// @warning halts program until copy is completed (calls `waitIdle()` on transfer queue)
        Result<Unit, ErrorMessage> end();
        
        StagedCopy(StagedCopy&) = delete;
        StagedCopy operator=(StagedCopy&) = delete;
    };
}