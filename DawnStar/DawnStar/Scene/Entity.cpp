#include <DawnStar/dspch.hpp>
#include <DawnStar/Scene/Entity.hpp>

namespace DawnStar
{
    Entity::Entity(entt::entity handle, Scene* scene)
        : m_EntityHandle(handle), m_Scene(scene)
    {
        
    }
} // namespace DawnStar
