#pragma once
#include "../utilities/common.h"
#include <unordered_map>

template <typename T>
class StringIdMap
{
private:
    std::unordered_map<StringId, T> map_ = std::unordered_map<StringId, T>();
public:
    const std::unordered_map<StringId, T>& map() const { return this->map_; }
    
    Option<T*> tryGet(StringId id) const
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
    
    T& get(StringId id) const
    {
        auto searchResult = this->map_.find(id);
        if (searchResult == this->map_.end())
        {
            panic("Couldn't find " + std::string(id) + ".");
        }
        return searchResult->second;
    }

    Result<Unit, std::string> tryAdd(StringId id, T item)
    {
        auto insertResult = this->map_.insert({id, std::move(item)});
        if (!insertResult.second)
        {
            return Err<std::string>("Tried to add " + std::string(id) + " multiple times.");
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
};