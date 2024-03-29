#pragma once

#include <glm/gtx/quaternion.hpp>

#include <DawnStar/Scene/Scene.hpp>
#include <DawnStar/Scene/Components.hpp>
#include <DawnStar/Debug/Profiler.hpp>

namespace DawnStar
{
    class Entity
    {
    public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) const
		{
			DS_PROFILE_SCOPE()

			DS_CORE_ASSERT(_scene, "Scene is null!")
			T& component = _scene->_registry.emplace_or_replace<T>(_entityHandle, std::forward<Args>(args)...);
			_scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		[[nodiscard]] T& GetComponent() const
		{
			DS_PROFILE_SCOPE()
	
			bool has_component = HasComponent<T>();
			DS_CORE_ASSERT(has_component, "Entity does not have component");
			return _scene->_registry.get<T>(_entityHandle);
		}

		template<typename T>
		[[nodiscard]] bool HasComponent() const
		{
			DS_PROFILE_SCOPE()

			DS_CORE_ASSERT(_scene, "Scene is null!")
			return _scene->_registry.all_of<T>(_entityHandle);
		}

		template<typename T>
		void RemoveComponent() const
		{
			DS_PROFILE_SCOPE()

			DS_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!")
			_scene->_registry.remove<T>(_entityHandle);
		}

		[[nodiscard]] UUID GetUUID() const { return GetComponent<IDComponent>().ID; }
		[[nodiscard]] std::string_view GetTag() const { return GetComponent<TagComponent>().Tag; }
		[[nodiscard]] TransformComponent& GetTransform() const { return GetComponent<TransformComponent>(); }
		[[nodiscard]] RelationshipComponent& GetRelationship() const { return GetComponent<RelationshipComponent>(); }

		[[nodiscard]] Entity GetParent() const
		{
			DS_PROFILE_SCOPE()

			if (!_scene)
				return {};

			const auto& rc = GetComponent<RelationshipComponent>();
			return rc.Parent != 0 ? _scene->GetEntity(rc.Parent) : Entity {};
		}
		void SetParent(Entity parent) const
		{
			DS_PROFILE_SCOPE()

			DS_CORE_ASSERT(_scene->_entityMap.contains(parent.GetUUID()), "Parent is not in the same scene as entity")
			Deparent();
			
			auto& rc = GetComponent<RelationshipComponent>();
			rc.Parent = parent.GetUUID();
			parent.GetRelationship().Children.emplace_back(GetUUID());
		}

		void Deparent() const
		{
			DS_PROFILE_SCOPE()

			auto& transform = GetRelationship();
			const UUID uuid = GetUUID();
			const Entity parentEntity = GetParent();
			
			if (!parentEntity)
				return;

			auto& parent = parentEntity.GetRelationship();
			for (auto it = parent.Children.begin(); it != parent.Children.end(); ++it)
			{
				if (*it == uuid)
				{
					parent.Children.erase(it);
					break;
				}
			}
			transform.Parent = 0;
		}

		// TODO: Gloab space에서의 x, y, z 좌표를 얻는 방법이 없음.
		
		[[nodiscard]] glm::mat4 GetWorldTransform() const
		{
			DS_PROFILE_SCOPE()

			const auto& transform = GetTransform();
			const auto& rc = GetRelationship();
			const Entity parent = _scene->GetEntity(rc.Parent);
			const glm::mat4 parentTransform = parent ? parent.GetWorldTransform() : glm::mat4(1.0f);
			return parentTransform * glm::translate(glm::mat4(1.0f), transform.Translation) * 
									 glm::toMat4(glm::quat(transform.Rotation)) * 
									 glm::scale(glm::mat4(1.0f), transform.Scale);
		}

		[[nodiscard]] glm::mat4 GetLocalTransform() const
		{
			DS_PROFILE_SCOPE()

			const auto& transform = GetTransform();
			return glm::translate(glm::mat4(1.0f), transform.Translation) * 
				   glm::toMat4(glm::quat(transform.Rotation)) * 
				   glm::scale(glm::mat4(1.0f), transform.Scale);
		}

		[[nodiscard]] glm::mat4 GetUIWorldTransform() const
		{
			DS_PROFILE_SCOPE()

			const auto& transform = GetTransform();
			const auto& layout = GetComponent<UI::LayoutComponent>();
			const auto& rc = GetRelationship();
			const Entity parent = _scene->GetEntity(rc.Parent);
			const glm::mat4 parentTransform = parent ? parent.GetUIWorldTransform() : glm::mat4(1.0f);
			
			// Scaling of UI object size only requires executing the lowest object.
			// So we add 'rc.Children.size() > 0' at the end
			return parentTransform * glm::translate(glm::mat4(1.0f), transform.Translation) * 
									 glm::toMat4(glm::quat(transform.Rotation)) * 
									 glm::translate(glm::mat4(1.0f), glm::vec3((0.5f - layout.Pivot.x) * transform.Scale.x,
																			   (0.5f - layout.Pivot.y) * transform.Scale.y, 0.0f)) *
				   					 glm::scale(glm::mat4(1.0f), transform.Scale);
		}

		[[nodiscard]] glm::mat4 GetUILocalTransform() const
		{
			DS_PROFILE_SCOPE()
			const auto& transform = GetTransform();
			const auto& layout = GetComponent<UI::LayoutComponent>();
			// Calcuate pivot based rotation transform matrix
			// mat = Translation * Rotation * PivotTranslation * Scale
			return glm::translate(glm::mat4(1.0f), transform.Translation) * 
				   glm::toMat4(glm::quat(transform.Rotation)) * 
				   glm::translate(glm::mat4(1.0f), glm::vec3((0.5f - layout.Pivot.x) * transform.Scale.x,
															 (0.5f - layout.Pivot.y) * transform.Scale.y, 0.0f)) *
				   glm::scale(glm::mat4(1.0f), transform.Scale);
		}

		[[nodiscard]] Scene* GetScene() const { return _scene; }

		operator bool() const { return _entityHandle != entt::null && _scene != nullptr && _scene->_registry.valid(_entityHandle); }
		operator entt::entity() const { return _entityHandle; }
		operator uint32_t() const { return static_cast<uint32_t>(_entityHandle); }

		bool operator==(const Entity& other) const { return _entityHandle == other._entityHandle && _scene == other._scene; }
		bool operator!=(const Entity& other) const { return _entityHandle != other._entityHandle || _scene != other._scene; }
        
	private:
		entt::entity _entityHandle = entt::null;
		Scene* _scene = nullptr;
    };
} // namespace DawnStar
