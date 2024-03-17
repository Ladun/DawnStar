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
        virtual void OnUpdate(Timestep ts, entt::registry& registry) override;

    private:
        // Update position and size of ui object
        void UpdateLayoutSystem(Timestep ts, entt::registry& registry);
        // Search for interactable objects in the current frame
        Entity UpdateInteractOjbect(Timestep ts, entt::registry& registry);
        // Interation with button
        void UpdateButtonSystem(Timestep ts, entt::registry& registry);
        // Update input texts
        void UpdateInputTextSystem(Timestep ts, entt::registry& registry);

    private:
        Entity _currentEntity;

        // For inputText;
        const float _blinkTime = 0.5f;
        float _curBlinkTime = 0.0f;
    };
} // namespace DawnStar
