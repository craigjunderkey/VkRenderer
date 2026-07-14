#pragma once
#include "types.h"
#include <vector>
#include <unordered_map>

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

template <typename K, typename V>
std::vector<const V*> mapValuesAsConstPtrs(const std::unordered_map<K, V>& map)
{
    std::vector<const V*> vec {};
    vec.reserve(map.size());
    for (const auto& pair : map)
    {
        const V* valPtr = &pair.second;
        vec.push_back(valPtr);
    }
    return std::move(vec);
}

template<typename K, typename V>
std::vector<const K*> mapKeysAsConstPtrs(const std::unordered_map<K, V>& map)
{
    std::vector<const K*> vec {};
    vec.reserve(map.size());
    for (const auto& pair : map)
    {
        const K* keyPtr = &pair.first;
        vec.push_back(keyPtr);
    }
    return std::move(vec);
}

template<typename K, typename V>
std::unordered_map<K, u32> idMapFromKeys(const std::unordered_map<K, V>& map)
{
    std::vector<const K*> idVec = mapKeysAsConstPtrs(map);
    
    auto idMap = std::unordered_map<K, u32>();
    for (u32 i = 0; i < idVec.size(); i++)
    {
        idMap.insert({ *idVec[i], i });
    }
    
    return std::move(idMap);
} 

template<typename K, typename V>
std::unordered_map<K, u32> idMapFromValues(const std::unordered_map<K, V>& map)
{
    std::vector<const K*> idVec = mapValuesAsConstPtrs(map);
    
    auto idMap = std::unordered_map<K, u32>();
    for (u32 i = 0; i < idVec.size(); i++)
    {
        idMap.insert({ *idVec[i], i });
    }
    
    return std::move(idMap);
} 