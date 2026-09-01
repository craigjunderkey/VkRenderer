#include "vulkan_memory_helper.h"

namespace HelloTriangleApplication::Utilities
{
    HostMemoryMapping::~HostMemoryMapping()
    {
        if (this->memory_ && this->memory_->isHostMapped() && this->data_)
        {
            this->memory_->memory().unmapMemory();
            this->memory_->isHostMapped_ = false;
        }
    }

    Result<HostMemoryMapping, ErrorMessage> DeviceMemoryBlock::map(vk::DeviceSize regionOffset, Option<vk::DeviceSize> regionSize)
    {
        assert(this->memory_ != nullptr);
        assert(!this->isHostMapped_);

        vk::DeviceSize trueRegionSize = this->size_;
        if (regionSize)
            trueRegionSize = regionSize.value();

        auto mapResult = castVkResult(this->memory_.mapMemory(regionOffset, trueRegionSize));
        CHECK_KEEP(mapResult);
        this->isHostMapped_ = true;

        return HostMemoryMapping(*this, mapResult.take(), trueRegionSize, regionOffset);
    }

    Result<u32, ErrorMessage> findMemoryType(vk::raii::PhysicalDevice& physicalDevice, vk::MemoryPropertyFlags properties, Option<u32> typeFilterMaybe = {})
    {
        vk::PhysicalDeviceMemoryProperties availableTypes = physicalDevice.getMemoryProperties();
        
        u32 typeFilter = typeFilterMaybe.value_or(0);

        for (u32 i = 0; i < availableTypes.memoryTypeCount; i++)
        {
            bool satisfiesTypeFilter = typeFilterMaybe && typeFilter & (1 << i);
            bool hasRequiredFeatures = (availableTypes.memoryTypes[i].propertyFlags & properties) == properties;

            if (satisfiesTypeFilter && hasRequiredFeatures)
            {
                return i;
            }
        }

        return ErrorMessage({ .channel = "vulkan" }, "Failed to find suitable memory type.");
    }

    Result<DeviceMemoryBlock, ErrorMessage> VulkanMemoryHelper::allocate(vk::DeviceSize allocSize,
        vk::MemoryPropertyFlags properties, Option<vk::MemoryRequirements> requirements)
    {
        // get memory type
        Option<u32> typeFilterMaybe = {};
        if (requirements)
        {
            typeFilterMaybe = requirements.value().memoryTypeBits;
        }

        auto findMemResult = findMemoryType(this->physicalDevice_, properties, typeFilterMaybe);
        CHECK_KEEP(findMemResult);
        u32 memType = findMemResult.take();

        // get allocation size
        vk::DeviceSize trueAllocSize = allocSize;
        if (requirements && requirements->size > allocSize)
        {
            trueAllocSize = requirements->size;
            warn({ .channel = "vulkan", .verbosity = 1},
                "Requested allocation of size {} bytes but {} bytes is required; increased allocation size to compensate.",
                allocSize, trueAllocSize
            );
        }

        // allocate memory
        vk::MemoryAllocateInfo memAllocInfo {
            .allocationSize = trueAllocSize,
            .memoryTypeIndex = memType
        };
        
        auto allocResult = castVkResult(this->device_.allocateMemory(memAllocInfo));
        CHECK_KEEP(allocResult);
        return DeviceMemoryBlock(allocResult.take(), trueAllocSize);
    }
}