#pragma once

#include <entt.hpp>

#include <DawnStar/Core/Timestep.hpp>
#include <DawnStar/Scene/Components.hpp>

namespace DawnStar
{
    class Scene;
    
    class SystemBase
    {
        friend class Scene;
    public:
        SystemBase() = default;
        SystemBase(const SystemBase& other) = delete;
        SystemBase(SystemBase&& other) = delete;
        ~SystemBase() = default;

    protected:
        virtual void OnUpdate(Timestep ts, entt::registry& registry) = 0;
    };
} // namespace DawnStar
