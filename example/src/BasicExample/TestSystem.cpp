#include <DawnStar/dspch.hpp>
#include "TestSystem.hpp"

namespace BasicExample
{
    void TestSystem::OnUpdate(DawnStar::Timestep ts, entt::registry& registry)
    {
        const auto view = registry.view<DawnStar::TransformComponent, DawnStar::SpriteRendererComponent>();
        for(auto &&[entity, transform, sprite] : view.each())
        {
            transform.Rotation.z += ts * 90 * (3.14 / 180);
        }
    }
}