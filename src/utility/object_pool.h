#pragma once
#include <cstdlib>
#include "types.h"

template <typename T>
union PoolObject
{
    PoolObject<T>* next;
    T data;
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

public:
    ObjectPool(u32 capacity, bool allowDynamicAllocation)
        : capacity_(capacity), allowDynamicAlloc_(allowDynamicAllocation)
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

    ~ObjectPool()
    {
        std::free(this->pool_);
    }

    constexpr bool isFull()
    {
        return this->head_ == nullptr;
    }

    // user is responsible for later calling `ObjectPool::free` on the pointer returned by `alloc`
    // the returned memory is not zero-initialized, so user is responsible for calling placement new
    T* alloc()
    {
        if (!this->isFull())
        {
            PoolObject<T>* objPtr = this->head_;
            this->head_ = this->head_->next;
            return &objPtr->data;
        }
        else if (this->allowDynamicAlloc_)
        {
            std::cerr << "WARNING: pool is full; resolved by allocating more memory.\n";
            T* objPtr = reinterpret_cast<T*>(std::malloc(sizeof(T)));
            return objPtr;
        }
        else
        {
            std::cout << "ERROR: pool is full; since dynamic allocations are disabled, error cannot be resolved.\n";
            assert(false);
            return nullptr;
        }
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
};