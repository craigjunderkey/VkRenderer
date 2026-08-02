#pragma once
#include <string>
#include <iostream>
#include <filesystem>

struct StringId
{
    friend std::hash<StringId>;

private:
    std::string id_;

public:
    StringId() : id_("") {};
    StringId(std::string id) : id_(id) {};
    StringId(const char* id) : id_(std::string(id)) {};
    
    bool operator==(const StringId& other) const
    {
        return this->id_ == other.id_;
    }

    bool isNull() const
    {
        return this->id_.empty();
    }

#ifndef NDEBUG
    const std::string& string() const
    {
        return this->id_;
    }
#endif

    friend std::ostream& operator<<(std::ostream& os, const StringId& id)
    {
        #ifndef NDEBUG
            return os << id.id_ << " (" << id.string() << ")";
        #else
            return os << id.id_;
        #endif
    }

    operator std::string() const
    {
        #ifndef NDEBUG
            return std::string(this->id_) + " (" + this->string() + ")";
        #else
            return std::string(this->id_);
        #endif
    }
};

namespace std
{
    template<> struct hash<StringId>
    {
        size_t operator()(const StringId& stringId) const
        {
            hash<std::string> hasher;
            return hasher(stringId.id_);
        }
    };
}