#include <dspch.hpp>
#include "TestSystem.hpp"

namespace BasicExample
{
    void TestSystem::OnUpdate(Timestep ts, entt::registry& registry)
    {
        const auto view = registry.view<TransformComponent, SpriteRendererComponent>();
        for(auto &&[entity, transform, sprite] : view.each())
        {
            float angle = glm::degrees(transform.Rotation.z);
            angle += 90 * ts;
            if(angle > 360)
            {
                angle -= 360;
            }

            transform.Rotation.z = glm::radians(angle);
        }
    }
}