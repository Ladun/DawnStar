#pragma once

#include <DawnStar/Scene/SystemBase.hpp>
#include <DawnStar/Scene/Entity.hpp>

namespace DawnStar::UI
{
    class UISystem: public SystemBase
    {
    public: 
        UISystem(Ref<Scene>& scene) : SystemBase(scene) {}
    protected:
        virtual void OnUpdate(DawnStar::Timestep ts, entt::registry& registry) override;

    private:
        void UpdateLayoutSystem(DawnStar::Timestep ts, entt::registry& registry);
        void UpdateButtonSystem(DawnStar::Timestep ts, entt::registry& registry);

    private:
        Entity _currentEntity;
    };
} // namespace DawnStar
