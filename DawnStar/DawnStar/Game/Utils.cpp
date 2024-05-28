#include <dspch.hpp>
#include <DawnStar/Game/Utils.hpp>
#include <DawnStar/Core/Application.hpp>
#include <DawnStar/Scene/Entity.hpp>

namespace DawnStar
{
    glm::vec2 ToScreenCoord(glm::vec2 mousePos)
    {
        // Original mouse pos coordinate system is based on left-top(0, 0), right-bottom(width, height)
        // But we need left-top(0, height), right-bottom(width, 0) coordinate system.

        auto windowWidth = Application::Get().GetWindow().GetWidth();
        auto windowHeight = Application::Get().GetWindow().GetHeight();
        return {mousePos.x, windowHeight - mousePos.y};
    }

    glm::vec2 ScreenToWorldCoord(glm::vec2 screenPos, Entity cameraEntity)
    {
		auto viewProjection = cameraEntity.GetComponent<CameraComponent>().Cam.GetProjection() * 
                              glm::inverse(cameraEntity.GetWorldTransform());
        auto windowWidth    = Application::Get().GetWindow().GetWidth();
        auto windowHeight   = Application::Get().GetWindow().GetHeight();
        // Get inverse projection of camera's ViewProjection Matrix
        viewProjection = glm::inverse(viewProjection);

        // ScreenPos based on left-top(0, height), right-bottom(width, 0) coordinate system.
        // Convert position system from original coordinate system 
        // to left-bottom(-1, -1), right-top(1, 1) system.
        glm::vec4 ret = {(screenPos.x / windowWidth) * 2.0f - 1.0f, 
                         (screenPos.y / windowHeight) * 2.0f - 1.0f,
                         0.0f, 1.0f};

        ret = viewProjection * ret;

        return {ret.x, ret.y};
    }
}