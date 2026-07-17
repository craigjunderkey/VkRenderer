#pragma once
#include "../utilities/common.h"
#include "../utilities/vulkan_common.h"
#include "render_object.h"
#include <vector>

namespace HelloTriangleApplication { class App; }

namespace HelloTriangleApplication::RenderLoop
{
    class GameLoop
    {
    private:
        u64 frame_ = 0;
        std::vector<RenderObject> scene_;
        App& app_;

    public:
        GameLoop(App& app) : app_(app) {}

        void initialize();
        void startFrame();
        void endFrame();
        void update(f32 delta);
        void draw(vk::raii::CommandBuffer& commandBuffer);

        static void submitDraw(App& app, const vk::raii::CommandBuffer& commandBuffer, const RenderObject& obj);

        inline std::vector<RenderObject>& scene() { return this->scene_; }
    };
}