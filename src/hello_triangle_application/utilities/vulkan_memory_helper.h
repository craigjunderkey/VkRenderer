#pragma once
#include "vulkan_common.h"

namespace HelloTriangleApplication::Utilities
{
    class DeviceMemoryObject;

    struct HostMemoryMapping
    {
        friend class DeviceMemoryObject;

    private:
        DeviceMemoryObject* memory_ = nullptr;
        void* data_ = nullptr;
        vk::DeviceSize size_ = 0;
        vk::DeviceSize offset_ = 0;

        HostMemoryMapping(DeviceMemoryObject& memory, void* data, vk::DeviceSize size, vk::DeviceSize offset)
            : memory_(&memory), data_(data), size_(size), offset_(offset)
        {
        }

    public:  
        HostMemoryMapping()
            : memory_(nullptr), data_(nullptr), size_(0), offset_(0)
        {
        }

        HostMemoryMapping& operator=(HostMemoryMapping&) = delete;
        HostMemoryMapping(HostMemoryMapping&) = delete;
        HostMemoryMapping& operator=(HostMemoryMapping&& other)
        {
            *this = HostMemoryMapping();
            std::swap(*this, other);
            return *this;
        }
        HostMemoryMapping(HostMemoryMapping&& other)
            : memory_(nullptr), data_(nullptr), size_(0), offset_(0)
        {
            std::swap(*this, other);
        }

        constexpr void* data() { return this->data_; }
        const void* data() const { return this->data_; }
        constexpr void* operator*() { return this->data_; }
        constexpr const void* operator*() const { return this->data_; }
        constexpr void* operator->() { return this->data_; }
        
        constexpr DeviceMemoryObject& memoryObject()
        {
            assert(this->memory_ != nullptr);
            return *this->memory_;
        }
        constexpr vk::DeviceSize regionSize() const { return this->size_; }
        constexpr vk::DeviceSize regionOffset() const { return this->offset_; }

        ~HostMemoryMapping();
    };
    
    // produces host memory mappings
    class DeviceMemoryObject
    {
        friend class VulkanMemoryHelper;
        friend class HostMemoryMapping;

    private:
        vk::raii::DeviceMemory memory_ = nullptr;
        vk::DeviceSize size_ = 0;
        bool isHostMapped_ = false;

        DeviceMemoryObject(vk::raii::DeviceMemory&& memory, vk::DeviceSize size)
            : memory_(std::move(memory)), size_(size), isHostMapped_(false)
        {
        }
    
    public:
        DeviceMemoryObject() = default;

        Result<HostMemoryMapping, ErrorMessage> map(vk::DeviceSize regionOffset, Option<vk::DeviceSize> regionSize = {});

        constexpr vk::raii::DeviceMemory& operator*() { return this->memory_; }
        constexpr const vk::raii::DeviceMemory& operator*() const { return this->memory_; }
        constexpr vk::raii::DeviceMemory* operator->() { return &this->memory_; }

        constexpr vk::raii::DeviceMemory& memory() { return this->memory_; }
        constexpr vk::DeviceSize size() { return this->size_; }
        constexpr bool isHostMapped() const { return this->isHostMapped_; }
    };

    // produces memory objects through an alloc method
    class VulkanMemoryHelper
    {
    private:
        vk::raii::PhysicalDevice& physicalDevice_;
        vk::raii::Device& device_;

    public:
        VulkanMemoryHelper() = delete;
        VulkanMemoryHelper(vk::raii::PhysicalDevice& physicalDevice, vk::raii::Device& device)
            : physicalDevice_(physicalDevice), device_(device)
        {
        }
        
        Result<DeviceMemoryObject, ErrorMessage> allocate(vk::DeviceSize allocSize,
            vk::MemoryPropertyFlags properties, Option<vk::MemoryRequirements> requirements = {});
    };
}