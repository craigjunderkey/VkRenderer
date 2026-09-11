#include "keyboard_controller.h"

namespace VkRenderer::Input
{
    bool KeyboardController::pressingInSet(const std::unordered_set<i32>& keys) const
    {
        for (i32 key : keys)
        {
            if (glfwGetKey(*this->window_, key) == GLFW_PRESS)
                return true;
        }
        return false;
    }
    
    bool KeyboardController::readAction(StringId name) const
    {
        auto result = this->actionMap_.find(name);
        assert(result != this->actionMap_.end());
        const std::unordered_set<i32>& keys = result->second;
        return this->pressingInSet(keys);
    }

    void KeyboardController::registerAction(StringId name, std::initializer_list<i32> keys)
    {
        // insert action into action map
        auto bindings = std::unordered_set<i32>();
        for (i32 key : keys)
        {
            bindings.insert(key);
        }
        this->actionMap_.insert({name, std::move(bindings)});

        // update current and previous keyboard states
        this->prevKeyboard_.insert({name, false});
        this->keyboard_.insert({name, this->readAction(name)});
    }

    void KeyboardController::startFrame()
    {
        this->keyboard_ = std::unordered_map<StringId, bool>();
        this->keyboard_.reserve(this->actionMap_.size());
        for (const auto& action : this->actionMap_)
        {
            this->keyboard_.insert({action.first, this->pressingInSet(action.second)});            
        }
    }

    void KeyboardController::endFrame()
    {
        this->prevKeyboard_ = std::move(this->keyboard_);
    }

    bool KeyboardController::pressing(StringId action) const
    {
        auto result = this->keyboard_.find(action);
        assert(result != this->keyboard_.end());
        return result->second;
    }

    bool KeyboardController::pressed(StringId action) const
    {
        auto currResult = this->keyboard_.find(action);
        assert(currResult != this->keyboard_.end());
        auto prevResult = this->prevKeyboard_.find(action);
        assert(prevResult != this->prevKeyboard_.end());
        return currResult->second && !prevResult->second;
    }

    bool KeyboardController::released(StringId action) const
    {
        auto currResult = this->keyboard_.find(action);
        assert(currResult != this->keyboard_.end());
        auto prevResult = this->prevKeyboard_.find(action);
        assert(prevResult != this->prevKeyboard_.end());
        return !currResult->second && prevResult->second;
    }

    f32 KeyboardController::axis(StringId neg, StringId pos) const
    {
        f32 val = 0.0f;
        if (this->pressing(neg))
            val -= 1.0f;
        if (this->pressing(pos))
            val += 1.0f;
        return val;
    }

    vec2 KeyboardController::vector(StringId negX, StringId posX, StringId negY, StringId posY) const
    {
        return VectorOps::normalize(vec2(this->axis(negX, posX), this->axis(negY, posY)));
    }
}