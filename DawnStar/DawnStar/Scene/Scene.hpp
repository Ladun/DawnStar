#pragma once

#include <entt.hpp>
#include <glm/glm.hpp>

#include <DawnStar/Core/Core.hpp>
#include <DawnStar/Core/UUID.hpp>
#include <DawnStar/Core/Timestep.hpp>


namespace DawnStar
{
	class Entity;

	using EntityLayer = uint16_t;
	struct EntityLayerData
	{
		std::string Name = "Layer";
		EntityLayer Flags = 0xFFFF;
		uint8_t Index = 1;
	};
	
	struct CameraData
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::mat4 ViewProjection;
		glm::vec3 Position;
	};

	class Scene
	{

	public:

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestriyEntity(Entity entity);
		Entity Duplicate(Entity entity);
		[[nodiscard]] bool HasEntity(UUID uuid) const;
		[[nodiscard]] Entity GetEntity(UUID uuid);

		void OnUpdate(Timestep ts);

		void OnViewportResize(uint32_t width, uint32_t height);
		[[nodiscard]] Entity GetPrimaryCameraEntity();

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void OnRender(const CameraData& cameraData);
	private:
		friend class Entity;

		entt::registry m_Registry;
		std::unordered_map<UUID, entt::entity> m_EntityMap;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	};
} // namespace DawnStar
