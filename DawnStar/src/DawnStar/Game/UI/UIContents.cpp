#include <dspch.hpp>
#include <DawnStar/Game/UI/UIContents.hpp>

#include <DawnStar/Core/Application.hpp>
#include <DawnStar/Scene/Components.hpp>

namespace DawnStar::UI
{
    
    Entity CreateInputText(Ref<Scene> scene, std::string name)
    {
        auto entity = scene->CreateEntity(name);
        auto& layout = entity.AddComponent<UI::LayoutComponent>();
        layout.AnchorMax = layout.AnchorMin = {0.5, 0.5};
        layout.Pivot = {0.5, 0.5};
        layout.Box = {0, 0, 80, 40};


        auto& inputText = entity.AddComponent<UI::InputTextComponent>();

        { // Text 
            auto ent = scene->CreateEntity("Text");
            ent.SetParent(entity);

            auto& layout = ent.AddComponent<UI::LayoutComponent>();
            layout.Pivot = {0.5, 0.5};
            // (l, t, r, b)
            layout.Box = {0, 0, 0, 0};

            auto& text = ent.AddComponent<TextComponent>();
            text.FontAsset = CreateRef<Font>("assets/fonts/godoRounded-L.ttf");
            text.FontSize = 32;
            text.Color = {1.0, 1.0, 1.0, 1.0};
            text.SetVerticalCenter();
            text.TextString = "Hello";

        }
        { // Background 
            auto ent = scene->CreateEntity("Background");
            ent.SetParent(entity);

            auto& layout = ent.AddComponent<UI::LayoutComponent>();
            layout.Pivot = {0.5, 0.5};
            layout.Box = {0, 5, 0, 5};

            auto& sprite = ent.AddComponent<UI::SpriteRendererComponent>();
            sprite.Color = {0.5, 0.5, 0.5, 1.0};
        }
        { // Caret 
            auto ent = scene->CreateEntity("Caret");
            ent.SetParent(entity);

            auto& layout = ent.AddComponent<UI::LayoutComponent>();
            layout.AnchorMin = {0.0, 0.0};
            layout.AnchorMax = {0.0, 1.0};
            layout.Pivot = {0.5, 0.5};
            // (x, t, w, b)
            layout.Box = {10, 10, 2, 10};

            auto& sprite = ent.AddComponent<UI::SpriteRendererComponent>();
            sprite.Color = {0.0, 0.0, 0.0, 1.0};
            sprite.Enable = false;
        }

        return entity;
    }
} // namespace DawnStar
