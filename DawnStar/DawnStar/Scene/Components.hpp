#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <DawnStar/Core/UUID.hpp>
#include <DawnStar/Core/Core.hpp>

#include <DawnStar/Renderer/Texture.hpp>
#include <DawnStar/Renderer/Camera.hpp>

namespace DawnStar
{
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

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture = nullptr;
		int32_t SortingOrder = 0;
		float TilingFactor = 1.0f;
	};

	struct CameraComponent
	{
		Camera Cam;
		bool Primary = true;
		bool FixedAspectRatio = false;
	};

	struct PrefabComponent
	{
		UUID ID;
	};

	// UI Component
	namespace UI
	{
		struct LayoutComponent // RequireComponents [TransformComponent]
		{
			glm::vec2 AnchorMin{0.0f, 0.0f};  // Anchor's minimum point normalized to parent size (0 to 1)
			glm::vec2 AnchorMax{1.0f, 1.0f};  // Anchor's maximum point normalized to parent size (0 to 1)
			glm::vec2 Pivot{0.5, 0.5};      // Pivot point normalized to the rectangle's size (0 to 1)

			float rotation = 0.0f;
			glm::vec2 Position;
			glm::vec4 Box; //( x, y, w, h) or (l, t, r, b)
		};

		struct SpriteRendererComponent
		{
			glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
			Ref<Texture2D> Texture = nullptr;
			int32_t SortingOrder = 0;
			float TilingFactor = 1.0f;
		};

		struct ButtonComponent
		{
			int temp = 0;
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
