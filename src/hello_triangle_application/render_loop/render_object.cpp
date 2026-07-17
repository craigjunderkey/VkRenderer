#include "render_object.h"

namespace HelloTriangleApplication::RenderLoop
{
    void RenderObject::addChild(RenderObject child, bool parentTransform)
    {
        this->children.emplace_back(std::move(child));
        if (parentTransform)
        {
            this->children.back().transform.parent = &this->transform;
        }
    }
}