#include <dspch.hpp>
#include <DawnStar/Game/UI/UISystem.hpp>
#include <DawnStar/Game/Utils.hpp>
#include <DawnStar/Scene/Scene.hpp>
#include <DawnStar/Scene/Entity.hpp>
#include <DawnStar/Scene/Components.hpp>
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
        // Reset state
        auto ent = UpdateInteractOjbect(ts, registry);
        if(_currentEntity && ent != _currentEntity)
        {
            if ( _currentEntity.HasComponent<UI::ButtonComponent>())
            {
                auto& button = _currentEntity.GetComponent<UI::ButtonComponent>();
                button._over = false;
                button._press = false;
                button._release = false;

                auto& sprite = _currentEntity.GetComponent<UI::SpriteRendererComponent>();
                sprite.Color = button.normalColor;
            }
            else if (_currentEntity.HasComponent<UI::InputTextComponent>())
            {
                auto text  =  _currentEntity.GetScene()->GetEntity(_currentEntity.GetRelationship().Children[0]);
                auto caret =  _currentEntity.GetScene()->GetEntity(_currentEntity.GetRelationship().Children[2]);

                caret.GetComponent<UI::SpriteRendererComponent>().Enable = false;
            }
        }
        _currentEntity = ent;
        UpdateButtonSystem(ts, registry);
        UpdateInputTextSystem(ts, registry);
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

    Entity UISystem::UpdateInteractOjbect(Timestep ts, entt::registry &registry)
    {
		DS_PROFILE_SCOPE();
        const auto view = registry.view<TransformComponent, UI::LayoutComponent>();
        Entity curEntity = Entity();

        // Find interacted object
        auto mousePos = ToScreenCoord(Input::GetMousePosition());
        if (mousePos.x < 0 || mousePos.x >= Application::Get().GetWindow().GetWidth() || 
            mousePos.y < 0 || mousePos.y >= Application::Get().GetWindow().GetHeight())
            return curEntity;

        mousePos.x -= Application::Get().GetWindow().GetWidth() / 2;
        mousePos.y -= Application::Get().GetWindow().GetHeight() / 2;
        for(auto &&[entity, transform, layout] : view.each())
        {
            if (layout.Enable == false)
                continue;

            if (layout.Interactable == false)
                continue;


            float left = transform.Translation.x - layout.Pivot.x * layout._size.x;
            float top = transform.Translation.y + (1 - layout.Pivot.y) * layout._size.y;
            float right = transform.Translation.x + (1 - layout.Pivot.x) * layout._size.x;
            float bottom = transform.Translation.y - layout.Pivot.y * layout._size.y;
            
            
            Entity ent = {entity, _scene.get()};
            // If the mouse is over an object
            if (left < mousePos.x  && mousePos.x < right &&
                bottom < mousePos.y && mousePos.y < top)
            {
                // Because current objects in registry is sorting by descending order
                // So we only care about the object in front.

                // TODO: sorting components in other code
                
                curEntity = ent;
                break;
            }
        }
        return curEntity;
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
            auto& text =  _currentEntity.GetScene()->GetEntity(_currentEntity.GetRelationship().Children[0]).GetComponent<TextComponent>();
            auto& caret =  _currentEntity.GetScene()->GetEntity(_currentEntity.GetRelationship().Children[2]).GetComponent<UI::SpriteRendererComponent>();

            // Caret update
            _curBlinkTime += ts;
            if(_curBlinkTime > _blinkTime * 2)
                _curBlinkTime -= _blinkTime * 2;

            if(_curBlinkTime > _blinkTime)
                caret.Enable = true;
            else
                caret.Enable = false;            


            // Text update
            // TODO: 텍스트를 입력받아서 처리하는 방식을 아래쳐럼 GetKeycode가 아닌, 
            //       Layer처럼 OnEvent로 키의 입력이 왔을 때 추가하는 방식으로 하는게 더 깔끔할 듯.
            // TODO: 텍스트 더하고 지우는 기능, caret 위치 조정하는거 추가
            if(Input::IsKeyDown(Key::Any))
            {
                auto keyCode = Input::GetKeyCode();

                text.TextString += keyCode;
            }

        }
    }

} // namespace DawnStar::UI

