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
		struct PositionComponent
		{
			float anchorMinX = 0.0f, anchorMinY = 0.f;
			float anchorMaxX, anchorMaxY;

			// 현재 오브젝트의 기준점의 위치
			float pivotX = 0.5f, pivotY = 0.5f;
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

		// 2D
		SpriteRendererComponent

		// 3D


	>;
} // namespace DawnStar
