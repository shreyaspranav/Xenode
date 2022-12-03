#pragma once

#include <Core.h>
#include <entt.hpp>

class SceneHierarchyPanel;

namespace Xen {
	class Entity;

	class XEN_API Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(double timestep);
		void OnCreate();

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

	private:
		entt::registry m_Registry;
		friend class Entity;
		friend class ::SceneHierarchyPanel;
	};

	class XEN_API Entity
	{
	public:
		Entity() {}
		Entity(Scene* scene) : m_Scene(scene) { m_Entity = m_Scene->m_Registry.create(); }
		Entity(entt::entity e, Scene* scene) : m_Entity(e), m_Scene(scene) {}
		~Entity() {}

		template<typename T, typename... Args>
		inline T& AddComponent(Args&&... args) { return m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...); }

		template<typename T>
		inline void DeleteComponent() { m_Scene->m_Registry.remove<T>(m_Entity); }

		template<typename T>
		inline T& GetComponent() { return m_Scene->m_Registry.get<T>(m_Entity); }

		template<typename... T>
		inline bool HasAnyComponent() { return m_Scene->m_Registry.any_of<T...>(m_Entity); }

		template<typename... T>
		inline bool HasAllComponent() { return m_Scene->m_Registry.all_of<T...>(m_Entity); }

		template<typename T, typename... Args>
		inline T& ReplaceComponent(Args&&... args) { return m_Scene->m_Registry.replace<T>(std::forward<Args>(args)...); }

		operator uint32_t() const { return (uint32_t)m_Entity; }
		operator entt::entity() const { return m_Entity; }

		bool IsNull() { return m_Entity == entt::null ? true : false; }
		//bool operator==(const Entity& other) { return other.m_Entity == m_Entity; }
		//bool operator!=(const Entity& other) { return other.m_Entity != m_Entity; }

	private:
		entt::entity m_Entity = entt::null;
		Scene* m_Scene;
	};
}

