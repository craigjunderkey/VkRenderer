#pragma once
#include <string>

struct StringId
{
    friend std::hash<StringId>;

public:
    StringId() : id_("") {};
    StringId(std::string id) : id_(id) {};
    
    bool operator==(const StringId& other) const
    {
        return this->id_ == other.id_;
    }

    bool isNull() const
    {
        return this->id_.empty();
    }

#ifndef NDEBUG
    const std::string& string()
    {
        return this->id_;
    }
#endif

private:
    std::string id_;
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