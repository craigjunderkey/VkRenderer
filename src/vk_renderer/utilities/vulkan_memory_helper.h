#pragma once
#include "vulkan_common.h"

namespace VkRenderer::Utilities
{
    class DeviceMemoryBlock;

    struct HostMemoryMapping
    {
        friend class DeviceMemoryBlock;

    private:
        DeviceMemoryBlock* memory_ = nullptr;
        void* data_ = nullptr;
        vk::DeviceSize size_ = 0;
        vk::DeviceSize offset_ = 0;

        HostMemoryMapping(DeviceMemoryBlock& memory, void* data, vk::DeviceSize size, vk::DeviceSize offset)
            : memory_(&memory), data_(data), size_(size), offset_(offset)
        {
        }

    public:  
        HostMemoryMapping()
            : memory_(nullptr), data_(nullptr), size_(0), offset_(0)
        {
        }

        void swap(HostMemoryMapping& other)
        {
            std::swap(this->memory_, other.memory_);
            std::swap(this->data_, other.data_);
            std::swap(this->size_, other.size_);
            std::swap(this->offset_, other.offset_);
        }
        
        HostMemoryMapping& operator=(HostMemoryMapping&) = delete;
        HostMemoryMapping(HostMemoryMapping&) = delete;
        HostMemoryMapping& operator=(HostMemoryMapping&& other)
        {
            this->memory_ = nullptr;
            this->data_ = nullptr;
            this->size_ = 0;
            this->offset_ = 0;
            this->swap(other);
            return *this;
        }
        HostMemoryMapping(HostMemoryMapping&& other)
            : memory_(nullptr), data_(nullptr), size_(0), offset_(0)
        {
            this->swap(other);
        }

        constexpr void* data()
        {
            assert(this->data_ != nullptr);
            return this->data_;
        }
        
        const void* data() const {
            assert(this->data_ != nullptr);
            return this->data_;
        }
        
        constexpr void* operator*() { return this->data(); }
        constexpr const void* operator*() const { return this->data(); }
        constexpr void* operator->() { return this->data(); }
        
        constexpr DeviceMemoryBlock& memoryObject()
        {
            assert(this->memory_ != nullptr);
            return *this->memory_;
        }
        constexpr vk::DeviceSize regionSize() const { return this->size_; }
        constexpr vk::DeviceSize regionOffset() const { return this->offset_; }

        ~HostMemoryMapping();
    };
    
    // produces host memory mappings
    class DeviceMemoryBlock
    {
        friend class VulkanMemoryHelper;
        friend class HostMemoryMapping;

    private:
        vk::raii::DeviceMemory memory_ = nullptr;
        vk::DeviceSize size_ = 0;
        bool isHostMapped_ = false;

        DeviceMemoryBlock(vk::raii::DeviceMemory&& memory, vk::DeviceSize size)
            : memory_(std::move(memory)), size_(size), isHostMapped_(false)
        {
        }
    
    public:
        DeviceMemoryBlock()
            : memory_(nullptr), size_(0), isHostMapped_(false)
        {
        }

        // mapping memory is an expensive operator, so keep memory mapped as long as you need it
        Result<HostMemoryMapping, ErrorMessage> map(vk::DeviceSize regionOffset = 0, Option<vk::DeviceSize> regionSize = {});

        constexpr vk::raii::DeviceMemory& memory()
        {
            assert(this->memory_ != nullptr);
            return this->memory_;
        }

        constexpr const vk::raii::DeviceMemory& memory() const
        {
            assert(this->memory_ != nullptr);
            return this->memory_;
        }

        DeviceMemoryBlock(DeviceMemoryBlock&) = delete;
        DeviceMemoryBlock& operator=(DeviceMemoryBlock&) = delete;

        void swap(DeviceMemoryBlock& other)
        {
            std::swap(this->memory_, other.memory_);
            std::swap(this->size_, other.size_);
            std::swap(this->isHostMapped_, other.isHostMapped_);
        }

        DeviceMemoryBlock(DeviceMemoryBlock&& other)
            : memory_(nullptr), size_(0), isHostMapped_(false)
        {
            this->swap(other);
        }

        DeviceMemoryBlock& operator=(DeviceMemoryBlock&& other)
        {
            this->memory_ = nullptr;
            this->size_ = 0;
            this->isHostMapped_ = false;
            this->swap(other);
            return *this;
        }

        constexpr vk::raii::DeviceMemory& operator*() { return this->memory(); }
        constexpr const vk::raii::DeviceMemory& operator*() const { return this->memory(); }
        constexpr vk::raii::DeviceMemory* operator->() { return &this->memory(); }

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
        
        Result<DeviceMemoryBlock, ErrorMessage> allocate(vk::DeviceSize allocSize,
            vk::MemoryPropertyFlags properties, Option<vk::MemoryRequirements> requirements = {});
    };

    inline void swap(DeviceMemoryBlock& lhs, DeviceMemoryBlock& rhs)
    {
        lhs.swap(rhs);
    }
}