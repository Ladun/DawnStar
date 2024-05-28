#include <dspch.hpp>
#include <DawnStar/Scene/Scene.hpp>

#include <DawnStar/Scene/Components.hpp>
#include <DawnStar/Scene/Entity.hpp>

#include <DawnStar/Renderer/Renderer2D.hpp>
#include <DawnStar/Renderer/Renderer3D.hpp>
#include <DawnStar/Renderer/RenderGraphData.hpp>
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

    void Scene::OnUpdate(Timestep ts, const Ref<RenderGraphData>& renderGraphData)
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
		OnRender(renderGraphData, cameraData);
		#pragma endregion
    }

    void Scene::OnRender(const Ref<RenderGraphData>& renderGraphData, const CameraData &cameraData)
    {
		DS_PROFILE_CATEGORY("Rendering", Profile::Category::Rendering);

		// Rendering 3D 
		std::vector<Entity> lights;
		{
			DS_PROFILE_SCOPE("Prepare Light Data")

			const auto view = _registry.view<LightComponent>();
			lights.reserve(view.size());
			for(auto &&[entity, lc] : view.each())
			{
				lights.emplace_back(entity, this);
			}			
		}

		Entity skyLight = {};
		{
			DS_PROFILE_SCOPE("Prepare Sky Light Data")

			const auto view = _registry.view<SkyLightComponent>();
			if(!view.empty())
			{
				skyLight = Entity(*view.begin(), this);
			}
		}

		Renderer3D::BeginScene(cameraData, skyLight, std::move(lights));
		{
			DS_PROFILE_SCOPE("Submit Mesh Data")

			const auto view = _registry.view<MeshComponent>();			
			Renderer3D::ReserveMeshes(view.size());
			
			for (auto &&[entity, meshComponent] : view.each())
			{
				if (meshComponent.MeshGeometry && meshComponent.MeshGeometry->GetSubmeshCount() != 0)
				{
					DS_CORE_ASSERT(meshComponent.MeshGeometry->GetSubmeshCount() > meshComponent.SubmeshIndex, "Trying to access submesh index that does not exist!")
					Renderer3D::SubmitMesh(Entity(entity, this).GetWorldTransform(), meshComponent.MeshGeometry->GetSubmesh(meshComponent.SubmeshIndex), meshComponent.CullMode);
				}
			}
		}
		Renderer3D::EndScene(renderGraphData);

		// TODO: 렌더링을 조금 비효율적으로 하는 거 같음. UI부분과 World 부분의 rendering 코드 수정 현재는 너무 중복된 느낌
		// Rendering 2D components
		Renderer2D::BeginScene(cameraData);
		{
			DS_PROFILE_SCOPE("Submit 2D Data");

			{
				const auto view = _registry.view<SpriteRendererComponent>();
				for (auto &&[entity, sprite] : view.each())
				{
					if(sprite.Enable == false)
						continue;
						
					Renderer2D::DrawQuad(Entity(entity, this).GetWorldTransform(), sprite.Texture, sprite.Color, sprite.TilingFactor);
				}
			}
		}
		Renderer2D::EndScene(renderGraphData);

		// Rendering UI component
		CameraData uiCamData = {};
		uiCamData.View 			 = glm::mat4(1.0f);
		uiCamData.Projection 	 = glm::mat4(1.0f);
		uiCamData.ViewProjection = _uiProjection;
		uiCamData.Position 		 = glm::vec4(1.0f);
		Renderer2D::BeginScene(uiCamData);
		{

			{
				DS_PROFILE_SCOPE("Submit UI Sprite Data");
				const auto view = _registry.view<UI::SpriteRendererComponent, UI::LayoutComponent>();
				for (auto &&[entity, sprite, layout] : view.each())
				{
					if(sprite.Enable == false)
						continue;
					
					if(layout.Enable == false)
						continue;			

					Renderer2D::DrawQuad(Entity(entity, this).GetUIWorldTransform() * glm::scale(glm::mat4(1.0f), glm::vec3(layout._size, 1.0f)), 
										 sprite.Texture, sprite.Color, sprite.TilingFactor);
				}
			}
			{
				DS_PROFILE_SCOPE("Submit UI Text Data");
				const auto view = _registry.view<TextComponent, UI::LayoutComponent>();
				for (auto &&[entity, text, layout] : view.each())
				{
					if(text.Enable == false)
						continue;
					
					if(layout.Enable == false)
						continue;		

					// Text rendering does not require size (including width and height) scaling.
					Renderer2D::DrawString(Entity(entity, this).GetUIWorldTransform(), layout._size, text);
				}
			}
		}
		Renderer2D::EndScene(renderGraphData);
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
		const float orthoLeft = -heightF * aspectRatio * 0.5f;
		const float orthoRight = heightF * aspectRatio * 0.5f;
		const float orthoBottom = -heightF * 0.5f;
		const float orthoTop = heightF * 0.5f;
		// (0, 0) left-bottom
		// const float orthoLeft = 0;
		// const float orthoRight = heightF * aspectRatio;
		// const float orthoBottom = 0;
		// const float orthoTop = heightF;

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
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<SkyLightComponent>(Entity entity, SkyLightComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<LightComponent>(Entity entity, LightComponent& component)
	{
	}
	
	template<>
	void Scene::OnComponentAdded<UI::LayoutComponent>(Entity entity, UI::LayoutComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<UI::SpriteRendererComponent>(Entity entity, UI::SpriteRendererComponent& component)
	{
		DS_CORE_ASSERT(entity.HasComponent<UI::LayoutComponent>(), "Doesn't have layout component");
	}
	
	template<>
	void Scene::OnComponentAdded<UI::ButtonComponent>(Entity entity, UI::ButtonComponent& component)
	{
		DS_CORE_ASSERT(entity.HasComponent<UI::SpriteRendererComponent>(), "Doesn't have sprite renderer component");

		auto& layout = entity.GetComponent<UI::LayoutComponent>();
		layout.Interactable = true;
	}

	template<>
	void Scene::OnComponentAdded<UI::InputTextComponent>(Entity entity, UI::InputTextComponent& component)
	{
		DS_CORE_ASSERT(entity.HasComponent<TextComponent>(), "Doesn't have TextComponent");
		
		auto& layout = entity.GetComponent<UI::LayoutComponent>();
		layout.Interactable = true;

	}
}