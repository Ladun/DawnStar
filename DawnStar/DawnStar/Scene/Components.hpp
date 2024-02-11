#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <DawnStar/Core/UUID.hpp>
#include <DawnStar/Core/Core.hpp>

#include <DawnStar/Renderer/Texture.hpp>
#include <DawnStar/Renderer/Camera.hpp>
#include <DawnStar/Renderer/Font.hpp>

namespace DawnStar
{
	struct BaseComponent
	{
		bool Enable = true;
	};

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const uint64_t id)
			: ID(id) {}
		operator uint64_t() { return ID; }
	};

	struct TagComponent
	{
		std::string Tag;
		uint16_t Layer = BIT(1);
		bool Enabled = true;

		bool handled = true;

		TagComponent() = default;
		explicit TagComponent(const std::string& tag)
			: Tag(tag) {}
	};
    
	class Entity;
	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	};
	
	struct RelationshipComponent
	{
		UUID Parent = 0;
		std::vector<UUID> Children;
	};

	struct SpriteRendererComponent : public BaseComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture = nullptr;
		int32_t SortingOrder = 0;
		float TilingFactor = 1.0f;
	};

	struct CameraComponent : public BaseComponent
	{
		Camera Cam;
		bool Primary = true;
		bool FixedAspectRatio = false;
	};

	struct PrefabComponent
	{
		UUID ID;
	};
	
	struct TextComponent // TODO: add align, ...
	{
		std::string TextString;
		Ref<Font> FontAsset = Font::GetDefault();
		glm::vec4 Color{ 1.0f };
		float Kerning = 0.0f;
		float LineSpacing = 0.0f;
	};

	// UI Component
	namespace UI
	{
		// RequireComponents [TransformComponent]
		struct LayoutComponent : public BaseComponent
		{
			glm::vec2 AnchorMin{0.0f, 0.0f};  // Anchor's minimum point normalized to parent size (0 to 1)
			glm::vec2 AnchorMax{1.0f, 1.0f};  // Anchor's maximum point normalized to parent size (0 to 1)
			glm::vec2 Pivot{0.5, 0.5};      // Pivot point normalized to the rectangle's size (0 to 1)

			glm::vec4 Box; //( x, y, w, h) or (l, t, r, b)
			float Rotation = 0.0f;

		};

		struct SpriteRendererComponent : public BaseComponent
		{
			glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
			Ref<Texture2D> Texture = nullptr;
			int32_t SortingOrder = 0;
			float TilingFactor = 1.0f;

			bool Interactable = false;
		};

		struct ButtonComponent : public BaseComponent
		{
			friend class UISystem;
		

			std::function<void()> onClick;
			glm::vec4 overColor{1.0f};
			glm::vec4 pressColor{1.0f};
			glm::vec4 normalColor{1.0f};

		public:
			bool _over;
			bool _press;
			bool _release;
		};
	}
	
	
	// etc...
	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents = ComponentGroup<
		TransformComponent,
		RelationshipComponent,
		PrefabComponent,
		CameraComponent,

		// UI
		UI::LayoutComponent,
		UI::SpriteRendererComponent,
		UI::ButtonComponent,

		// 2D
		SpriteRendererComponent

		// 3D


	>;
} // namespace DawnStar
