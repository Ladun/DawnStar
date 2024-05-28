#include <dspch.hpp>
#include <DawnStar/Scene/Entity.hpp>

namespace DawnStar
{
    Entity::Entity(entt::entity handle, Scene* scene)
        : _entityHandle(handle), _scene(scene)
    {
        
    }
} // namespace DawnStar
