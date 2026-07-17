#include "keyboard_controller.h"

namespace HelloTriangleApplication::Input
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
        const std::unordered_set<i32>& keys = this->actionMap_.find(name)->second;
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
        return this->keyboard_.find(action)->second;
    }

    bool KeyboardController::pressed(StringId action) const
    {
        return this->keyboard_.find(action)->second && !this->prevKeyboard_.find(action)->second;
    }

    bool KeyboardController::released(StringId action) const
    {
        return !this->keyboard_.find(action)->second && this->prevKeyboard_.find(action)->second;
    }
}