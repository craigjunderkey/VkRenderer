#include "json_helper.h"

void JsonHelper::setProperty(const char* key, f32& property)
{
    property = static_cast<f32>(this->data_[key]);
}

void JsonHelper::setProperty(const char* key, vec2& property)
{
    auto& value = this->data_[key];
    property = vec2(static_cast<f32>(value[0]), static_cast<f32>(value[1]));
}

void JsonHelper::setProperty(const char* key, vec3& property)
{
    auto& value = this->data_[key];
    property = vec3(static_cast<f32>(value[0]), static_cast<f32>(value[1]), static_cast<f32>(value[2]));    
}

void JsonHelper::setProperty(const char* key, StringId& property)
{
    auto& value = this->data_[key];
    property = StringId(value);
}

void JsonHelper::setProperty(const char* key, bool& property)
{
    property = static_cast<bool>(this->data_[key]);
}
