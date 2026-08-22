#pragma once
#include "types.h"
#include "log/log_error.h"
#include <unordered_map>
#include <vector>

template <typename T>
class StringIdMap
{
private:
    std::unordered_map<StringId, T> map_ = std::unordered_map<StringId, T>();

public:
    StringId name = "unnamed";

    StringIdMap() = default;
    StringIdMap(StringId name) : name(name) {}

    Option<T*> tryGet(StringId id)
    {
        auto searchResult = this->map_.find(id);
        if (searchResult == this->map_.end())
        {
            return {};
        }
        else
        {
            return Option<T*>(&searchResult->second);
        }
    }
    
    T& get(StringId id)
    {
        auto searchResult = this->map_.find(id);
        if (searchResult == this->map_.end())
        {
            LogUtils::err({}, "Couldn't find {} in {}.", id, this->name);
            panic();
        }
        return searchResult->second;
    }

    Option<const T*> tryGet(StringId id) const
    {
        auto searchResult = this->map_.find(id);
        if (searchResult == this->map_.end())
        {
            return {};
        }
        else
        {
            return Option<const T*>(&searchResult->second);
        }
    }
    
    const T& get(StringId id) const
    {
        auto searchResult = this->map_.find(id);
        if (searchResult == this->map_.end())
        {
            LogUtils::err({}, "Couldn't find {} in {}.", id, this->name);
            panic();
        }
        return searchResult->second;
    }

    Result<Unit, LogUtils::ErrorMessage> tryAdd(StringId id, T item)
    {
        if (id == StringId::NONE)
            return LogUtils::ErrorMessage({}, "Tried to add a mapping for StringId::NONE to {}; \
this value is reserved for null/empty values.", this->name);

        auto insertResult = this->map_.insert({id, std::move(item)});
        if (!insertResult.second)
        {
            return LogUtils::ErrorMessage({}, "Failed to insert {0} in {1}, likely due to inserting {0} multiple times", id, this->name);
        }
        else
        {
            return Ok();
        }
    }

    void add(StringId id, T item)
    {
        auto addResult = this->tryAdd(id, std::move(item));
        addResult.unwrap();
    }

    Option<T> tryRemove(StringId id)
    {
        auto extractResult = this->map_.extract(id);
        if (extractResult.empty())
        {
            return {};
        }
        else
        {
            return std::move(extractResult.mapped());
        }
    }

    T remove(StringId id)
    {
        auto removeResult = this->tryRemove(id);
        if (!removeResult)
        {
            LogUtils::err({}, "Couldn't find {} in {}.", id, this->name);
            panic();
        }
        return std::move(removeResult).value();
    }

    inline void clear()
    {
        this->map_.clear();
    }

    inline void reserve(u32 size)
    {
        this->map_.reserve(size);
    }

    inline u32 size() const
    {
        return this->map_.size();
    }

    // iterators
    auto begin() { return this->map_.begin(); }
    auto end() { return this->map_.end(); }
    auto begin() const { return this->map_.begin(); }
    auto end() const { return this->map_.end(); }
    auto cbegin() { return this->map_.cbegin(); }
    auto cend() { return this->map_.cend(); }

    std::vector<StringId> makeIdVec() const
    {
        std::vector<StringId> vec;
        vec.reserve(this->map_.size());
        for (const auto& entry : this->map_)
        {
            vec.push_back(entry.first);
        } 
        return std::move(vec);
    }

    std::vector<const T*> makePtrVec(const std::vector<StringId>& ids) const
    {
        std::vector<const T*> vec;
        vec.reserve(this->map_.size());
        for (StringId id : ids)
        {
            vec.push_back(&this->get(id));
        }
        return std::move(vec);
    }

    static StringIdMap<u32> makeIndexMap(const std::vector<StringId>& ids)
    {
        StringIdMap<u32> map;
        map.reserve(ids.size());
        for (u32 i = 0; i < ids.size(); i++)
        {
            map.add(ids[i], i);
        }
        return std::move(map);
    }
};