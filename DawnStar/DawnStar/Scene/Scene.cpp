#include <DawnStar/dspch.hpp>
#include <DawnStar/Scene/Scene.hpp>

#include <DawnStar/Scene/Components.hpp>
#include <DawnStar/Scene/Entity.hpp>

#include <DawnStar/Renderer/Renderer2D.hpp>
#include <DawnStar/Renderer/Font.hpp>

namespace DawnStar
{
	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, std::unordered_map<UUID, entt::entity> enttMap)
	{
		([&]()
		{
			auto view = src.view<Component>();
			for (auto e : view)
			{
				UUID uuid = src.get<IDComponent>(e).ID;
				entt::entity dstEnttID = enttMap.at(uuid);
				const auto& component = src.get<Component>(e);
				dst.emplace_or_replace<Component>(dstEnttID, component);
			}
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, std::unordered_map<UUID, entt::entity> enttMap)
	{
		CopyComponent<Component...>(dst, src, enttMap);
	}

	template<typename... Component>
	static void CopyComponent(entt::registry& dst, Entity srcEntity, Entity dstEntity)
	{
		([&]()
		{
			if (srcEntity.HasComponent<Component>())	// Currently not support duplicating ScriptComponent
			{
				const auto& component = srcEntity.GetComponent<Component>();
				dst.emplace_or_replace<Component>(dstEntity, component);
			}
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, Entity srcEntity, Entity dstEntity)
	{
		CopyComponent<Component...>(dst, srcEntity, dstEntity);
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		DS_PROFILE_SCOPE();

		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		DS_PROFILE_SCOPE();

		Entity entity = {_registry.create(), this};
		_entityMap.emplace(uuid, entity);

		entity.AddComponent<IDComponent>(uuid);

		entity.AddComponent<RelationshipComponent>();
		entity.AddComponent<TransformComponent>();

		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty()? "Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		DS_PROFILE_SCOPE();

		entity.Deparent();
		auto children = entity.GetComponent<RelationshipComponent>().Children;
		for(const auto& child: children)
		{
			if(Entity childEntity = GetEntity(child))
			{
				DestroyEntity(childEntity);
			}
		}
		bool hasSprite = entity.HasComponent<SpriteRendererComponent>();

		_entityMap.erase(entity.GetUUID());
		_registry.destroy(entity);
	}

    Entity Scene::Duplicate(Entity entity)
    {
		DS_PROFILE_SCOPE();

		std::string name = entity.GetComponent<TagComponent>().Tag;
		Entity duplicate = CreateEntity(name);

		CopyComponent(AllComponents{}, _registry, entity, duplicate);
		
		return duplicate;
    }

    bool Scene::HasEntity(UUID uuid) const
    {
		DS_PROFILE_SCOPE()
		
		return _entityMap.contains(uuid);
    }

    Entity Scene::GetEntity(UUID uuid)
    {
		DS_PROFILE_SCOPE()

		const auto& it = _entityMap.find(uuid);
		if (it != _entityMap.end())
			return { it->second, this };

		return {};
    }

    void Scene::AddSystem(Ref<SystemBase> system)
    {
		DS_PROFILE_SCOPE();

		_systems.push_back(system);
    }

    void Scene::OnUpdate(Timestep ts)
    {
		DS_PROFILE_SCOPE();

		#pragma region System updating
		{
			for(Ref<SystemBase> system: _systems)
			{
				system->OnUpdate(ts, _registry);
			}
		}
		#pragma endregion

		#pragma region Rendering
		CameraData cameraData = {};
		{
			DS_PROFILE_CATEGORY("Camera", Profile::Category::Camera);

			const Entity cameraEntity = GetPrimaryCameraEntity();
			if (cameraEntity)
			{
				cameraData.View 			= glm::inverse(cameraEntity.GetWorldTransform());
				cameraData.Projection 		= cameraEntity.GetComponent<CameraComponent>().Cam.GetProjection();
				cameraData.ViewProjection 	= cameraData.Projection * cameraData.View;
				cameraData.Position 		= cameraEntity.GetTransform().Translation;
			}
		}
		OnRender(cameraData);
		#pragma endregion
    }

    void Scene::OnRender(const CameraData &cameraData)
    {
		DS_PROFILE_CATEGORY("Rendering", Profile::Category::Rendering);

		// Rendering 2D components
		Renderer2D::BeginScene(cameraData.ViewProjection);
		{
			DS_PROFILE_SCOPE("Submit 2D Data");

			const auto view = _registry.view<SpriteRendererComponent>();
			for (auto &&[entity, sprite] : view.each())
			{
				if(sprite.Enable == false)
					continue;
					
				Renderer2D::DrawQuad(Entity(entity, this).GetWorldTransform(), sprite.Texture, sprite.Color, sprite.TilingFactor);
			}
			
			// Draw text
			{
				auto view = _registry.view<TextComponent>();
				for (auto &&[entity, text] : view.each())
				{
					Renderer2D::DrawString(Entity(entity, this).GetWorldTransform(), text);
				}
			}
		}
		Renderer2D::EndScene();

		// Rendering UI component
		Renderer2D::BeginScene(_uiProjection);
		{
			DS_PROFILE_SCOPE("Submit UI Data");

			const auto view = _registry.view<UI::SpriteRendererComponent, UI::LayoutComponent>();
			for (auto &&[entity, sprite, layout] : view.each())
			{
				if(sprite.Enable == false)
					continue;

				const auto& transform = Entity(entity, this).GetTransform();
				// Calcuate pivot based rotation transform matrix
				// mat = Translation * Rotation * PivotTranslation * Scale
				glm::mat4 mat = glm::translate(glm::mat4(1.0f), transform.Translation) * 
								glm::toMat4(glm::quat(transform.Rotation)) * 
								glm::translate(glm::mat4(1.0f), glm::vec3((0.5f - layout.Pivot.x) * transform.Scale.x,
                                                                          (0.5f - layout.Pivot.y) * transform.Scale.y, 0.0f)) * 
								glm::scale(glm::mat4(1.0f), transform.Scale);				

				Renderer2D::DrawQuad(mat, sprite.Texture, sprite.Color, sprite.TilingFactor);
			}

		}
		Renderer2D::EndScene();
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
		DS_PROFILE_SCOPE();

		_viewportWidth = width;
		_viewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		const auto view = _registry.view<CameraComponent>();
		for( const auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Cam.SetViewportSize(width, height);
		}

		// Update projection matrix for ui rendering
		float heightF = static_cast<float>(height);
		float aspectRatio = static_cast<float>(width) / heightF;
		// (0, 0) center
		// const float orthoLeft = -heightF * aspectRatio * 0.5f;
		// const float orthoRight = heightF * aspectRatio * 0.5f;
		// const float orthoBottom = -heightF * 0.5f;
		// const float orthoTop = heightF * 0.5f;
		// (0, 0) left-bottom
		const float orthoLeft = 0;
		const float orthoRight = heightF * aspectRatio;
		const float orthoBottom = 0;
		const float orthoTop = heightF;

		_uiProjection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop,
									-1.0f, 1.0f);
    }

    Entity Scene::GetPrimaryCameraEntity()
    {
		DS_PROFILE_CATEGORY("Camera", Profile::Category::Camera);

		const auto view = _registry.view<CameraComponent>();
		for(auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if(camera.Primary)
				return {entity, this};
		}
		return {};
    }
	
    template <typename T>
    void Scene::OnComponentAdded(Entity entity, T &component)
    {
		// static_assert ( false);
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Cam.SetViewportSize(_viewportWidth, _viewportHeight);
	}
	
	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TextComponent>(Entity entity, TextComponent& component)
	{
	}
	
	template<>
	void Scene::OnComponentAdded<UI::LayoutComponent>(Entity entity, UI::LayoutComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<UI::SpriteRendererComponent>(Entity entity, UI::SpriteRendererComponent& component)
	{
		DS_CORE_ASSERT(entity.HasComponent<UI::LayoutComponent>(), "Don't have layout component");
	}

	template<>
	void Scene::OnComponentAdded<UI::ButtonComponent>(Entity entity, UI::ButtonComponent& component)
	{
		DS_CORE_ASSERT(entity.HasComponent<UI::SpriteRendererComponent>(), "Don't have sprite renderer component");

		auto& sprite = entity.GetComponent<UI::SpriteRendererComponent>();
		sprite.Interactable = true;
	}
}