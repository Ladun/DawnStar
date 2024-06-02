#pragma once

#include <entt.hpp>
#include <glm/glm.hpp>

#include <DawnStar/Core/Core.hpp>
#include <DawnStar/Core/UUID.hpp>
#include <DawnStar/Core/Timestep.hpp>
#include <DawnStar/Scene/SystemBase.hpp>

#define SCENE_ADDCOMPONENT_FUNC_HEAD(ComponentType) \
template<> \
void DawnStar::Scene::OnComponentAdded<ComponentType>(DawnStar::Entity entity, ComponentType& component)


namespace DawnStar
{
	class Entity;
	struct CameraData;
	struct RenderGraphData;

	using EntityLayer = uint16_t;
	struct EntityLayerData
	{
		std::string Name = "Layer";
		EntityLayer Flags = 0xFFFF;
		uint8_t Index = 1;
	};
	
	struct CameraData;
	struct RenderGraphData;

	class Scene : public std::enable_shared_from_this<Scene>
	{
	public:

		/// @brief created entity has only ID, Relationship, Transform, Tag component
		/// @param name 
		/// @return 
		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		Entity Duplicate(Entity entity);
		
		bool HasEntity(UUID uuid) const;
		Entity GetEntity(UUID uuid);
		
		void AddSystem(Ref<SystemBase> system);

		void OnUpdate(Timestep ts, const Ref<RenderGraphData>& renderGraphData);

		void OnViewportResize(uint32_t width, uint32_t height);
		Entity GetPrimaryCameraEntity();

		template<typename Component>
		void SortComponents(std::function<bool(const Component&, const Component&)> compare)
		{			
			_registry.sort<Component>(compare);
		}

	public:
		uint32_t GetViewportWidth() { return _viewportWidth; }
		uint32_t GetViewportHeight() { return _viewportHeight; }

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void OnRender(const Ref<RenderGraphData>& renderGraphData, const CameraData& cameraData);

	private:
		entt::registry _registry;
		std::unordered_map<UUID, entt::entity> _entityMap;

		std::vector<Ref<SystemBase>> _systems;

		uint32_t _viewportWidth = 0, _viewportHeight = 0;
		glm::mat4 _uiProjection;
		
		friend class Entity;
		friend class SystemBase;
		friend class ObjectListPanel;
	};
} // namespace DawnStar