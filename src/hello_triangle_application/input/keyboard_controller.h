#pragma once
#include "../window.h"
#include "../utilities/common.h"
#include <unordered_map>
#include <unordered_set>

namespace HelloTriangleApplication::Input
{
    class KeyboardController
    {
    private:
        Window& window_;
        std::unordered_map<StringId, std::unordered_set<i32>> actionMap_;
        std::unordered_map<StringId, bool> keyboard_;
        std::unordered_map<StringId, bool> prevKeyboard_;

        bool readAction(StringId name) const;
        bool pressingInSet(const std::unordered_set<i32>& keys) const;

    public:
        KeyboardController(Window& window) : window_(window)
        {
            this->actionMap_ = std::unordered_map<StringId, std::unordered_set<i32>>();
            this->keyboard_ = std::unordered_map<StringId, bool>();
            this->prevKeyboard_ = std::unordered_map<StringId, bool>();
        }

        void registerAction(StringId name, std::initializer_list<i32> keys);
        void startFrame();
        void endFrame();
        bool pressing(StringId action) const;
        bool pressed(StringId action) const;
        bool released(StringId action) const;
    };
}