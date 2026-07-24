#pragma once
#include <nlohmann/json.hpp>
#include "../utilities/common.h"

class JsonHelper
{
private:
    const nlohmann::json& data_;

public:
    JsonHelper(const nlohmann::json& data) : data_(data) {}

    void setProperty(const char* key, f32& property);
    void setProperty(const char* key, vec2& property);
    void setProperty(const char* key, vec3& property);
    void setProperty(const char* key, StringId& property);
    void setProperty(const char* key, bool& property);

    template<typename T>
    bool trySetProperty(const char* key, T& property)
    {
        if (this->data_.contains(key))
        {
            this->setProperty(key, property);
            return true;
        }
        return false;
    }
};