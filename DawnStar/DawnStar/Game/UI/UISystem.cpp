#include <DawnStar/dspch.hpp>
#include <DawnStar/Game/UI/UISystem.hpp>
#include <DawnStar/Scene/Scene.hpp>
#include <DawnStar/Scene/Entity.hpp>
#include <DawnStar/Core/Application.hpp>

namespace DawnStar::UI
{
    
    void UISystem::OnUpdate(DawnStar::Timestep ts, entt::registry& registry)
    {
        // Update ui components position
        const auto view = registry.view<TransformComponent, UI::LayoutComponent, RelationshipComponent>();
        for(auto &&[entity, transform, layout, relationShip] : view.each())
        {   
            glm::vec2 parentSize;
            if(_scene->HasEntity(relationShip.Parent))
            {
                auto parent = _scene->GetEntity(relationShip.Parent);
                parentSize = {_scene->GetViewportWidth(), _scene->GetViewportHeight()};
            }
            else
            {
                parentSize = {_scene->GetViewportWidth(), _scene->GetViewportHeight()};
            }
            
            // Calculate X-axis
            if(layout.AnchorMin.x == layout.AnchorMax.x)
            {
                transform.Scale.x = layout.Box.z;
                transform.Translation.x = layout.Box.x + transform.Scale.x * 0.5f - layout.Pivot.x * transform.Scale.x;
            }
            else
            {
                transform.Scale.x = parentSize.x - (layout.Box.x + layout.Box.z)
                                    - (layout.AnchorMin.x * parentSize.x + (1 - layout.AnchorMax.x) * parentSize.x);
                
                // Anchor left position + adjusted position by pivot
                transform.Translation.x = layout.AnchorMin.x * parentSize.x + layout.Box.x + 
                                            transform.Scale.x * 0.5f;
            }

            if(layout.AnchorMin.y == layout.AnchorMax.y)
            {
                transform.Scale.y = layout.Box.w;
                transform.Translation.y = layout.Box.y + transform.Scale.y * 0.5f - layout.Pivot.y * transform.Scale.y;
            }
            else
            {
                transform.Scale.y = parentSize.y - (layout.Box.y + layout.Box.w)
                                    - (layout.AnchorMin.y * parentSize.y + (1 - layout.AnchorMax.y) * parentSize.y);
                
                // Anchor left position + adjusted position by pivot
                transform.Translation.y = layout.AnchorMin.y * parentSize.y + layout.Box.w + 
                                            transform.Scale.y * 0.5f;
            }

        }
    }
} // namespace DawnStar::UI
