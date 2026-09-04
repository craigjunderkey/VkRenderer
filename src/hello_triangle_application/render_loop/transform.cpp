#include "../hello_triangle_application.h"
#include "transform.h"

#include <numbers>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

constexpr f32 QUARTER_PI = std::numbers::pi / 4.0f;

namespace HelloTriangleApplication::RenderLoop
{
    mat4 Trs::model() const
    {
        mat4 model = mat4(1.0f);
        model = glm::translate(std::move(model), this->t);
        model = glm::eulerAngleXYZ(this->r.x, this->r.y, this->r.z) * std::move(model);
        model = glm::scale(std::move(model), this->s);
        return std::move(model);
    }

    mat4 Transform::local() const
    {
        mat4 model = mat4(1.0f);
        model = glm::translate(std::move(model), this->t);
        model = glm::eulerAngleXYZ(this->r.x, this->r.y, this->r.z) * std::move(model);
        model = glm::scale(std::move(model), this->s);
        return std::move(model);
    }

    mat4 Transform::global() const
    {
        if (this->parent)
        {
            return this->parent.value()->global() * this->local();
        }
        else
        {
            return this->local();
        }
    }

    Trs Transform::trs() const
    {
        return Trs {
            .t = this->t,
            .r = this->r,
            .s = this->s
        };
    }

    void Transform::setFromTrs(const Trs& trs)
    {
        this->t = trs.t;
        this->r = trs.r;
        this->s = trs.s;
    }

    void createProjectionTransform(App& app)
    {
        app.projection = glm::perspective(
            QUARTER_PI,
            static_cast<f32>(app.swapchain.getExtent().width) / static_cast<f32>(app.swapchain.getExtent().height),
            0.1f, 10.0f
        );
        app.projection[1][1] *= -1;
    }
}