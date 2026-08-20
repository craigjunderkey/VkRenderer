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

    static void internStringPrehashed(std::string_view string, u32 hash)
    {
        strings_.try_emplace(hash, string);
    }

public:
    static constexpr u32 NONE = 0;

    u32 id = NONE;

    constexpr StringId() : id(NONE) {};
    constexpr StringId(u32 id) : id(id) {};

    constexpr StringId(std::string_view string)
        : id(Crc32::hash(string.data(), string.length()))
    {
    #ifndef NDEBUG
        internStringPrehashed(string, this->id);
    #endif
    }

    constexpr StringId(const char* string)
        : id(Crc32::hash(string))
    {
    #ifndef NDEBUG
        internStringPrehashed(string, this->id);
    #endif
    }

    constexpr StringId(const std::string& string)
        : id(Crc32::hash(string.data(), string.length()))
    {
    #ifndef NDEBUG
        internStringPrehashed(string, this->id);
    #endif
    }
        
    constexpr operator u32() const
    {
        return this->id;
    }
    
    constexpr bool operator==(const StringId& other) const
    {
        return this->id == other.id;
    }

    constexpr bool operator==(u32 other) const
    {
        return this->id == other;
    }

    constexpr operator bool() const
    {
        return this->id != NONE;
    }
    
    constexpr bool isNull() const
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
        static constexpr std::string NONE_STRING = "NULL";
        if (id.isNull()) return NONE_STRING;

        auto searchResult = strings_.find(id);
        if (searchResult == strings_.end())
        {
            std::cerr << "Tried to find string for " << id.id << " but it wasn't interned.\n";
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