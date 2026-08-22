#pragma once
#include "types.h"
#include <vector>
#include <unordered_map>
#include <format>

// ostream overloads
namespace glm
{
    inline std::ostream& operator<<(std::ostream& os, const vec2& vec)
    {
        return os << "[" << vec.x << ", " << vec.y << "]";
    }

    inline std::ostream& operator<<(std::ostream& os, const vec3& vec)
    {
        return os << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
    }

    inline std::ostream& operator<<(std::ostream& os, const vec4& vec)
    {
        return os << "[" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << "]";
    }
}

namespace std
{
    template<>
    struct formatter<filesystem::path>
    {
        constexpr auto parse(auto& ctx) const
        {
            return ctx.begin();
        }

        auto format(const filesystem::path& path, auto& ctx) const
        {
            return std::format_to(ctx.out(), "{}", path.c_str());
        }
    };
}

template <typename T>
void printVecAsOrderedList(const std::vector<T>& vec)
{
    for (u32 i = 0; i < vec.size(); i++)
    {
        std::cout << i + 1 << ". " << vec[i] << std::endl;
    }
}

template <typename K, typename V>
void printMap(const std::unordered_map<K, V> map)
{
    for (const auto& pair : map)
    {
        std::cout << "(" << pair.first << ", " << pair.second << ")\n";
    }
}