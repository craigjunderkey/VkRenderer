#pragma once
#include <cstdlib>
#include "types.h"
#include "macros.h"

template <typename T>
union PoolObject
{
    PoolObject<T>* next;
    T data;
};

template<typename T>
class ObjectPool;

template<typename T>
void freePoolPtr(ObjectPool<T>* pool, T* ptr);

/*
An owning smart pointer a la `std::unique_ptr` which contains a data pointer and a pointer to the
pool which created it. Upon destruction, `PoolPtr` calls the data's destructor then `ObjectPool::free`.
*/
template <typename T>
struct PoolPtr
{
private:
    ObjectPool<T>* pool_ = nullptr;
    T* ptr_ = nullptr;

public:
    PoolPtr() : pool_(nullptr), ptr_(nullptr) {}
    
    PoolPtr(ObjectPool<T>& pool, T* ptr) : ptr_(ptr)
    {
        assert(ptr != nullptr);
        this->pool_ = &pool;
    }

    PoolPtr(PoolPtr& other) = delete;
    
    PoolPtr(PoolPtr&& other) : pool_(nullptr), ptr_(nullptr)
    {
        std::swap(this->ptr_, other.ptr_);
        std::swap(this->pool_, other.pool_);
    }
    
    PoolPtr& operator =(PoolPtr& other) = delete;
    
    PoolPtr& operator =(PoolPtr&& other)
    {
        std::swap(this->ptr_, other.ptr_);
        std::swap(this->pool_, other.pool_);
        return *this;
    };

    ~PoolPtr()
    {
        if (this->ptr_)
        {
            assert(this->pool_ != nullptr);
            this->ptr_->~T();
            freePoolPtr<T>(pool_, ptr_);
        }
    }

    constexpr T* operator->()
    {
        return this->ptr_;
    }

    constexpr T operator*()
    {
        return *this->ptr_;
    }

    constexpr operator T*() const
    {
        return this->ptr_;
    }

    constexpr operator bool() const
    {
        return this->ptr_ != nullptr;
    }

    constexpr T* get() const
    {
        return this->ptr_;
    }

    // converts this into a raw pointer
    // that is, `release` returns this's data pointer then nullifies this's data and pool pointers
    T* release()
    {
        T* ptr = this->ptr_;
        this->ptr_ = nullptr;
        this->pool_ = nullptr;
        return ptr;
    }
};

/*
A generic implementation of the Object Pool pattern (https://gameprogrammingpatterns.com/object-pool.html)
with optional dynamic allocation.
*/
template <typename T>
class ObjectPool
{
private:
    PoolObject<T>* pool_;
    PoolObject<T>* head_;
    u32 capacity_;
    bool allowDynamicAlloc_;
    StringId name_ = "unnamed";

public:
    ObjectPool(u32 capacity, bool allowDynamicAllocation, StringId name = "unnamed")
        : capacity_(capacity), allowDynamicAlloc_(allowDynamicAllocation), name_(name)
    {
        assert(capacity > 0);
        // allocate and clear memory
        this->pool_ = reinterpret_cast<PoolObject<T>*>(std::malloc(capacity * sizeof(PoolObject<T>)));
        
        // initialize linked list
        this->head_ = &pool_[0];
        for (u32 i = 0; i < capacity; i++)
        {
            if (i + 1 < capacity)
            {
                this->pool_[i].next = &this->pool_[i + 1];
            }
            else
            {
                this->pool_[i].next = nullptr;
            }
        }
    }

    ObjectPool(ObjectPool&) = delete;
    ObjectPool& operator=(ObjectPool&) = delete;
    ObjectPool(ObjectPool&& other) : pool_(nullptr), head_(nullptr)
    {
        std::swap(this->pool_, other.pool_);
        std::swap(this->head_, other.head_);
        std::swap(this->capacity_, other.capacity_);
        std::swap(this->allowDynamicAlloc_, other.allowDynamicAlloc_);
        std::swap(this->name_, other.name_);
    }
    ObjectPool& operator=(ObjectPool&& other)
    {
        this->pool_ = nullptr;
        this->head_ = nullptr;
        std::swap(this->pool_, other.pool_);
        std::swap(this->head_, other.head_);
        std::swap(this->capacity_, other.capacity_);
        std::swap(this->allowDynamicAlloc_, other.allowDynamicAlloc_);
        std::swap(this->name_, other.name_);
        return *this;
    }

    ~ObjectPool()
    {
        if (this->pool_)
        {
            std::free(this->pool_);
        }
    }

    constexpr bool isFull()
    {
        return this->head_ == nullptr;
    }

    // user is responsible for later calling `ObjectPool::free` on the pointer returned by `alloc`
    // the returned memory is not zero-initialized, so user is responsible for calling placement new
    T* allocRaw()
    {
        if (!this->isFull())
        {
            PoolObject<T>* objPtr = this->head_;
            this->head_ = this->head_->next;
            return &objPtr->data;
        }
        else if (this->allowDynamicAlloc_)
        {
            std::cerr << "WARNING: pool " << this->name_ << " is full; resolved by allocating more memory.\n";
            T* objPtr = reinterpret_cast<T*>(std::malloc(sizeof(T)));
            return objPtr;
        }
        else
        {
            std::cout << "ERROR: pool " << this->name_
                << " is full; since dynamic allocations are disabled, error cannot be resolved.\n";
            assert(false);
            return nullptr;
        }
    }

    PoolPtr<T> alloc()
    {
        T* ptr = this->allocRaw();
        return PoolPtr<T>(*this, ptr);
    }

    PoolPtr<T> alloc(T&& val)
    {
        T* ptr = this->allocRaw();
        *ptr = std::move(val);
        return PoolPtr<T>(*this, ptr);
    }
    
    // `free` does not call `*objPtr`'s destructor, so user is encouraged to free the object
    // in its destructor
    void free(T* objPtr)
    {
        PoolObject<T>* poolObjPtr = reinterpret_cast<PoolObject<T>*>(objPtr);
        if (poolObjPtr >= this->pool_ && poolObjPtr < this->pool_ + this->capacity_)
        {
            poolObjPtr->next = this->head_;
            this->head_ = poolObjPtr;
        }
        else
        {
            // in this case, `objPtr` was dynamically allocated, so it isn't part of the pool
            // hence, it's just a piece of stray memory and we can free it
            std::free(objPtr);
        }
    }

    void free(PoolPtr<T>& poolPtr)
    {
        T* ptr = poolPtr.release();
        this->free(ptr);
    }
};

template <typename T>
inline void freePoolPtr(ObjectPool<T>* pool, T* ptr)
{
    if (pool && ptr)
        pool->free(ptr);
}
