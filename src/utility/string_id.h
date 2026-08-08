#pragma once
#include <string>
#include <iostream>
#include <unordered_map>
#include "crc32.h"

struct StringId
{
    friend std::hash<StringId>;

private:
    static std::unordered_map<u32, std::string> strings_;

    static void internStringPrehashed(const char* string, u32 hash)
    {
        strings_.try_emplace(hash, string);
    }

public:
    static constexpr u32 NONE = 0;

    u32 id = NONE;

    StringId() : id(NONE) {};
    StringId(u32 id) : id(id) {};

    StringId(const std::string& id)
        : id(Crc32::hash(id.data(), id.length()))
    {
    #ifndef NDEBUG
        internStringPrehashed(id.data(), this->id);
    #endif
    }
    
    StringId(const char* id)
        : id(Crc32::hash(id))
    {
    #ifndef NDEBUG
        internStringPrehashed(id, this->id);
    #endif
    }
    
    constexpr operator u32() const
    {
        return this->id;
    }
    
    bool operator==(const StringId& other) const
    {
        return this->id == other.id;
    }

    constexpr operator bool() const
    {
        return this->id != NONE;
    }
    
    bool isNull() const
    {
        return this->id == NONE;
    }

    static StringId internString(const char* string)
    {
        u32 hash = Crc32::hash(string);
        strings_.try_emplace(hash, string);
        return hash;
    }

    static const std::string& getString(StringId id)
    {
        auto searchResult = strings_.find(id);
        if (searchResult == strings_.end())
        {
            std::cerr << "Tried to find string for " << id << " but it wasn't interned.\n";
            std::exit(EXIT_FAILURE);
        }
        return searchResult->second;
    }

     const std::string& string() const
    {
        return getString(this->id);
    }

    friend std::ostream& operator<<(std::ostream& os, const StringId& id)
    {
        #ifndef NDEBUG
            return os << id.id << " (" << id.string() << ")";
        #else
            return os << id.id;
        #endif
    }

    operator std::string() const
    {
        #ifndef NDEBUG
            return std::to_string(this->id) + " (" + this->string() + ")";
        #else
            return std::to_string(this->id);
        #endif
    }

    auto operator<=>(const StringId& other) const = default;
    
};

namespace std
{
    template<> struct hash<StringId>
    {
        size_t operator()(const StringId& stringId) const
        {
            hash<u32> hasher;
            return hasher(stringId.id);
        }
    };
}