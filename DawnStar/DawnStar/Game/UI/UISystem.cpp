#include <DawnStar/dspch.hpp>
#include <DawnStar/Game/UI/UISystem.hpp>
#include <DawnStar/Game/Utils.hpp>
#include <DawnStar/Scene/Scene.hpp>
#include <DawnStar/Scene/Entity.hpp>
#include <DawnStar/Core/Application.hpp>
#include <DawnStar/Core/Input.hpp>

namespace DawnStar::UI
{
    
    void UISystem::OnUpdate(DawnStar::Timestep ts, entt::registry& registry)
    {
        // Update ui components position

        UpdateLayoutSystem(ts, registry);
        UpdateButtonSystem(ts, registry);
    }

    void UISystem::UpdateLayoutSystem(DawnStar::Timestep ts, entt::registry &registry)
    {
		DS_PROFILE_SCOPE();
        const auto view = registry.view<TransformComponent, UI::LayoutComponent, RelationshipComponent>();
        for(auto &&[entity, transform, layout, relationShip] : view.each())
        {   
            glm::vec2 parentSize;
            if(_scene->HasEntity(relationShip.Parent))
            {
                auto parentTransform = _scene->GetEntity(relationShip.Parent).GetTransform();
                parentSize = {parentTransform.Scale.x, parentTransform.Scale.y};
            }
            else
            {
                parentSize = {_scene->GetViewportWidth(), _scene->GetViewportHeight()};
            }
            
            // Calculate X-axis
            if(layout.AnchorMin.x == layout.AnchorMax.x)
            {
                transform.Scale.x = layout.Box.z;
                // Currently, it is a coordinate system based on the center, 
                // so to convert it into a coordinate system where the lower-left corner is the reference, 
                // we use 1 - pivot.
                transform.Translation.x = layout.Box.x;
                
            }
            else
            {
                transform.Scale.x = parentSize.x - (layout.Box.x + layout.Box.z)
                                    - (layout.AnchorMin.x * parentSize.x + (1 - layout.AnchorMax.x) * parentSize.x);
                
                // Anchor left position + adjusted position by pivot
                transform.Translation.x = layout.AnchorMin.x * parentSize.x + layout.Box.x + 
                                          transform.Scale.x * layout.Pivot.x;
            }

            if(layout.AnchorMin.y == layout.AnchorMax.y)
            {
                transform.Scale.y = layout.Box.w;
                transform.Translation.y = layout.Box.y;
            }
            else
            {
                transform.Scale.y = parentSize.y - (layout.Box.y + layout.Box.w)
                                    - (layout.AnchorMin.y * parentSize.y + (1 - layout.AnchorMax.y) * parentSize.y);
                
                // Anchor left position + adjusted position by pivot
                transform.Translation.y = layout.AnchorMin.y * parentSize.y + layout.Box.w + 
                                          transform.Scale.y * layout.Pivot.y;
            }
            transform.Rotation.z = glm::radians(layout.Rotation);

        }
    }

    void UISystem::UpdateButtonSystem(DawnStar::Timestep ts, entt::registry &registry)
    {
		DS_PROFILE_SCOPE();
        const auto view = registry.view<TransformComponent, UI::SpriteRendererComponent,
                                        UI::LayoutComponent>();


        _currentEntity = Entity();
        auto mousePos = ToScreenCoord(Input::GetMousePosition());
        for(auto &&[entity, transform, sprite, layout] : view.each())
        {   
            float left = transform.Translation.x - layout.Pivot.x * transform.Scale.x;
            float top = transform.Translation.y + (1 - layout.Pivot.y) * transform.Scale.y;
            float right = transform.Translation.x + (1 - layout.Pivot.x) * transform.Scale.x;
            float bottom = transform.Translation.y - layout.Pivot.y * transform.Scale.y;

            bool intersection = left < mousePos.x  && mousePos.x < right &&
                                bottom < mousePos.y && mousePos.y < top;

            Entity ent = {entity, _scene.get()};
            // If the mouse is over an object
            if (intersection)
            {
                if(!_currentEntity && sprite.Enable && sprite.Interactable)
                {
                    _currentEntity = ent;
                }
            }

            if (!ent.HasComponent<UI::ButtonComponent>())
                continue;
            auto& button = ent.GetComponent<UI::ButtonComponent>();

            button._release = false;
            // Checking is mouse over or not
            if(_currentEntity == ent)
            {
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
            else
            {
                button._over = false;
                sprite.Color = button.normalColor;
            }
        }
    }
} // namespace DawnStar::UI

