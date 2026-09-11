#pragma once
#include "../utilities/common.h"
#include <iostream>
#include <fstream>

// format: vec.size(), vec.data()
template <typename T>
void writeVec(std::ofstream& out, const std::vector<T>& vec)
{
    u32 size = vec.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(u32));
    out.write(reinterpret_cast<const char*>(vec.data()), sizeof(T) * size);
}

// format: vec.size(), vec.data()
template <typename T>
void readVec(std::ifstream& in, std::vector<T>& vec)
{
    u32 size;
    in.read(reinterpret_cast<char*>(&size), sizeof(u32));
    vec.reserve(size);
    in.read(reinterpret_cast<char*>(vec.data()), sizeof(T) * size);
    vec.resize(size);
}