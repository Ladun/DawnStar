#pragma once

#include <DawnStar/Scene/SystemBase.hpp>

namespace DawnStar::UI
{
    class UISystem: public SystemBase
    {
    public: 
        UISystem(Ref<Scene>& scene) : SystemBase(scene) {}
    protected:
        virtual void OnUpdate(DawnStar::Timestep ts, entt::registry& registry) override;
    };
} // namespace DawnStar
