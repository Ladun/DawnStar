#include <DawnStar/dspch.hpp>
#include <DawnStar/Scene/Scene.hpp>

#include <DawnStar/Scene/Components.hpp>
#include <DawnStar/Scene/Entity.hpp>
#include <DawnStar/Scene/ScriptableEntity.hpp>

#include <DawnStar/Renderer/Renderer2D.hpp>

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

		Entity entity = {m_Registry.create(), this};
		m_EntityMap.emplace(uuid, entity);

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

		// Remove script
		if (entity.HasComponent<ScriptComponent>())
		{
			auto& script = entity.GetComponent<ScriptComponent>();
			script.DestroyScript(&script);
		}

		entity.Deparent();
		auto children = entity.GetComponent<RelationshipComponent>().Children;
		for(const auto& child: children)
		{
			if(Entity childEntity = GetEntity(child))
			{
				DestroyEntity(childEntity);
			}
		}

		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
	}

    Entity Scene::Duplicate(Entity entity)
    {
		DS_PROFILE_SCOPE();

		std::string name = entity.GetComponent<TagComponent>().Tag;
		Entity duplicate = CreateEntity(name);

		CopyComponent(AllComponents{}, m_Registry, entity, duplicate);
		
		return duplicate;
    }

    bool Scene::HasEntity(UUID uuid) const
    {
		DS_PROFILE_SCOPE()
		
		return m_EntityMap.contains(uuid);
    }

    Entity Scene::GetEntity(UUID uuid)
    {
		DS_PROFILE_SCOPE()

		const auto& it = m_EntityMap.find(uuid);
		if (it != m_EntityMap.end())
			return { it->second, this };

		return {};
    }

    void Scene::OnUpdate(Timestep ts)
    {
		DS_PROFILE_SCOPE();

		#pragma region Scripts
		{
			m_Registry.view<ScriptComponent>().each([=, this](auto entity, auto& sc)
				{
					if (!sc.Instance)
					{
						sc.Instance = sc.InstantiateScript();
						sc.Instance->m_Entity = Entity{ entity, this };
						sc.Instance->OnCreate();
					}

					sc.Instance->OnUpdate(ts);
				});
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

			const auto view = m_Registry.view<SpriteRendererComponent>();
			for (auto &&[entity, sprite] : view.each())
			{
				Renderer2D::DrawQuad(Entity(entity, this).GetWorldTransform(), sprite.Texture, sprite.Color, sprite.TilingFactor);
			}
		}
		Renderer2D::EndScene();

		// Rendering UI component
		Renderer2D::BeginScene(m_UIProjeciton);
		{
			DS_PROFILE_SCOPE("Submit UI Data");

			const auto view = m_Registry.view<UISpriteRendererComponent>();
			for (auto &&[entity, sprite] : view.each())
			{
				Renderer2D::DrawQuad(Entity(entity, this).GetWorldTransform(), sprite.Texture, sprite.Color, sprite.TilingFactor);
			}

		}
		Renderer2D::EndScene();
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
		DS_PROFILE_SCOPE();

		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		const auto view = m_Registry.view<CameraComponent>();
		for( const auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Cam.SetViewportSize(width, height);
		}

		// Update projection matrix for ui rendering
		float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		const float orthoLeft = -10 * aspectRatio * 0.5f;
		const float orthoRight = 10 * aspectRatio * 0.5f;
		const float orthoBottom = -10 * 0.5f;
		const float orthoTop = 10 * 0.5f;

		m_UIProjeciton = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop,
									-1.0f, 1.0f);
    }

    Entity Scene::GetPrimaryCameraEntity()
    {
		DS_PROFILE_CATEGORY("Camera", Profile::Category::Camera);

		const auto view = m_Registry.view<CameraComponent>();
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
		component.Cam.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
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
	void Scene::OnComponentAdded<UISpriteRendererComponent>(Entity entity, UISpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
		
	}
}