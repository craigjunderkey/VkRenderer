#pragma once
#include "transform.h"
#include "animation.h"

namespace VkRenderer::RenderLoop
{
    class RenderObject
    {
    public:
        StringId mesh;
        StringId material;
        Transform transform;
        std::unique_ptr<Animation> animation;
        std::vector<RenderObject> children {};

        RenderObject() = default;
        
        RenderObject(StringId mesh, StringId material, Transform transform, std::unique_ptr<Animation> anim = nullptr)
            : mesh(mesh), material(material), transform(transform), animation(std::move(anim))
        {
        }

        void addChild(RenderObject child, bool parentTransform = true);
    };
}