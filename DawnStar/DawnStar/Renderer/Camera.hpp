#pragma once

#include <glm/glm.hpp>

namespace DawnStar
{
    class Camera
    {
    public:
        Camera();

        glm::mat4 GetViewProjectionMatrix();
    };
} // namespace DawnStar
