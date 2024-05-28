#pragma once

#include <glm/vec2.hpp>


namespace DawnStar
{
    class Entity;

    glm::vec2 ToScreenCoord(glm::vec2 mousePos);

    glm::vec2 ScreenToWorldCoord(glm::vec2 screenPos, Entity cameraEntity);
}