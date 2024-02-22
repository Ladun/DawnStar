#include <DawnStar/dspch.hpp>
#include <DawnStar/Game/UI/UISystem.hpp>
#include <DawnStar/Game/Utils.hpp>
#include <DawnStar/Scene/Scene.hpp>
#include <DawnStar/Scene/Entity.hpp>
#include <DawnStar/Core/Application.hpp>
#include <DawnStar/Core/Input.hpp>
#include "UISystem.hpp"

namespace DawnStar::UI
{
    
    void UISystem::OnUpdate(Timestep ts, entt::registry& registry)
    {
        // Update ui components position

        // TODO: Ui system implementation
        //      UI Layout으로 for문을 돌면서 interacting이 되는 오브젝트 중에서 최상위 오브젝트 별도로 저장함
        //      추후 Button이나 InputText에 키보드나 마우스로 접근하는 것은 별도로 처리
        //      InputText는 선택되면 입력 깜빡이는 거 추가.

        // TODO: Scene이 처음 load되었을 떄 Button 색이 흰색으로 표현됨. 
        //       이를 보완하기 위해서 처음 load할 떄 색상을 설정해주는 거 추가.


        UpdateLayoutSystem(ts, registry);
        UpdateInteractOjbect(ts, registry);
        UpdateButtonSystem(ts, registry);
    }

    void UISystem::UpdateLayoutSystem(Timestep ts, entt::registry &registry)
    {
		DS_PROFILE_SCOPE();
        const auto view = registry.view<TransformComponent, UI::LayoutComponent, RelationshipComponent>();
        for(auto &&[entity, transform, layout, relationShip] : view.each())
        {   
            glm::vec2 parentSize;
            if(_scene->HasEntity(relationShip.Parent))
            {
                auto parentLayout = _scene->GetEntity(relationShip.Parent).GetComponent<UI::LayoutComponent>();
                parentSize = {parentLayout._size.x, parentLayout._size.y};
            }
            else
            {
                parentSize = {_scene->GetViewportWidth(), _scene->GetViewportHeight()};
            }
            
            // Calculate X-axis
            if(layout.AnchorMin.x == layout.AnchorMax.x)
            {
                layout._size.x = layout.Box.z;
                transform.Translation.x = layout.Box.x + (layout.AnchorMax.x - 0.5f) * parentSize.x;
                
            }
            else
            {
                layout._size.x = parentSize.x - (layout.Box.x + layout.Box.z)
                                 - (layout.AnchorMin.x * parentSize.x + (1 - layout.AnchorMax.x) * parentSize.x);
                
                transform.Translation.x = ((layout.AnchorMin.x - (1.0f - layout.AnchorMax.x)) * parentSize.x 
                                           + (layout.Box.x - layout.Box.z))* 0.5f;
            }

            if(layout.AnchorMin.y == layout.AnchorMax.y)
            {
                layout._size.y = layout.Box.w;
                transform.Translation.y = layout.Box.y + (layout.AnchorMax.y - 0.5f) * parentSize.y;
            }
            else
            {
                layout._size.y = parentSize.y - (layout.Box.y + layout.Box.w)
                                 - (layout.AnchorMin.y * parentSize.y + (1 - layout.AnchorMax.y) * parentSize.y);
                
                transform.Translation.y = ((layout.AnchorMin.y - (1.0f - layout.AnchorMax.y)) * parentSize.y 
                                           + (layout.Box.y - layout.Box.w)) * 0.5f;
            }
            transform.Scale = layout.Scale;
            transform.Rotation.z = glm::radians(layout.Rotation);

        }
    }

    void UISystem::UpdateInteractOjbect(Timestep ts, entt::registry &registry)
    {
		DS_PROFILE_SCOPE();
        const auto view = registry.view<TransformComponent, UI::LayoutComponent>();
        
        // Reset status
        if(_currentEntity && _currentEntity.HasComponent<UI::ButtonComponent>())
        {
            auto& button = _currentEntity.GetComponent<UI::ButtonComponent>();
            button._over = false;
            button._press = false;
            button._release = false;

            auto& sprite = _currentEntity.GetComponent<UI::SpriteRendererComponent>();
            sprite.Color = button.normalColor;
        }
        _currentEntity = Entity();


        // Find interacted object
        auto mousePos = ToScreenCoord(Input::GetMousePosition());
        for(auto &&[entity, transform, layout] : view.each())
        {
            if (layout.Enable == false)
                continue;

            if (layout.Interactable == false)
                continue;

            float left = transform.Translation.x - layout.Pivot.x * transform.Scale.x;
            float top = transform.Translation.y + (1 - layout.Pivot.y) * transform.Scale.y;
            float right = transform.Translation.x + (1 - layout.Pivot.x) * transform.Scale.x;
            float bottom = transform.Translation.y - layout.Pivot.y * transform.Scale.y;
            
            
            Entity ent = {entity, _scene.get()};
            // If the mouse is over an object
            if (left < mousePos.x  && mousePos.x < right &&
                bottom < mousePos.y && mousePos.y < top)
            {
                // Because current objects in registry is sorting by descending order
                // So we only care about the object in front.

                // TODO: sorting components in other code
                
                _currentEntity = ent;
                break;
            }
        }
    }

    void UISystem::UpdateButtonSystem(DawnStar::Timestep ts, entt::registry &registry)
    {
		DS_PROFILE_SCOPE();
        if(_currentEntity && _currentEntity.HasComponent<UI::ButtonComponent>())
        {
            auto& button = _currentEntity.GetComponent<UI::ButtonComponent>();
            if(button.Enable == false)
                return;

            auto& sprite = _currentEntity.GetComponent<UI::SpriteRendererComponent>();
            button._release = false;

            if(!button._over)
            {
                button._over = true;
                sprite.Color = button.overColor;
            }

            // Button action only on the object you are currently interacting with
            if(button._press && Input::IsMouseButtonUp(Mouse::ButtonLeft))
            {
                button._press = false;
                button._release = true;
                sprite.Color = button.overColor;

                button.onClick();
            }

            if(Input::IsMouseButtonDown(Mouse::ButtonLeft))
            {
                button._press = true;
                sprite.Color = button.pressColor;
            }                
        }
    }

    void UISystem::UpdateInputTextSystem(DawnStar::Timestep ts, entt::registry &registry)
    {
		DS_PROFILE_SCOPE();
        // Input text need multiple objects
        //  text object
        //  background
        //  blink

        if(_currentEntity && _currentEntity.HasComponent<UI::InputTextComponent>())
        {        
            _curBlinkTime += ts;

            // updating text
            // TODO: 이거 키코드 받는 방식을 나중에서는 OnEvent로 UISystem용으로 만들어서 하든지 해야할 듯.
            if(Input::IsKey(Key::Any))
            {
                auto keyCode = Input::GetKeyCode();

                
            }

        }
    }

} // namespace DawnStar::UI

