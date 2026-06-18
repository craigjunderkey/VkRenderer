#pragma once
#include <iostream>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "rust_types.h"
#include "string_id.h"

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using f32 = glm::float32_t;
using f64 = glm::float64_t;

using usize = size_t;

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;


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