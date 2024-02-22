#pragma once

#include <DawnStar/Scene/SystemBase.hpp>
#include <DawnStar/Scene/Entity.hpp>

namespace DawnStar::UI
{

    class UISystem: public SystemBase
    {
    public: 
        UISystem(Ref<Scene>& scene) : SystemBase(scene) {}    

        // TODO: 함수 선언 위치가 애매함 UISystem 내부 클래스에 종속되는 것보다는 
        //       이런 것을 만드는 클래스를 하나 새로 만드는 것이 좋을 듯.
        static Entity CreateInputText(Ref<Scene> scene, std::string name="Input Text");   

    protected:
        virtual void OnUpdate(Timestep ts, entt::registry& registry) override;

    private:
        // Update position and size of ui object
        void UpdateLayoutSystem(Timestep ts, entt::registry& registry);
        // Search for interactable objects in the current frame
        void UpdateInteractOjbect(Timestep ts, entt::registry& registry);
        // Interation with button
        void UpdateButtonSystem(Timestep ts, entt::registry& registry);
        // Update input texts
        void UpdateInputTextSystem(Timestep ts, entt::registry& registry);

    private:
        Entity _currentEntity;

        // For inputText;
        const float _blinkTime = 0.5f;
        float _curBlinkTime;
    };
} // namespace DawnStar
