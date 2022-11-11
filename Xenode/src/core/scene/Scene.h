#pragma once

#include <Core.h>
#include <entt.hpp>

#include "core/renderer/Texture.h"


namespace Xen {
	class Entity;

	class XEN_API Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(double timestep);
		void OnCreate();

		Entity CreateEntity(const std::string& name = std::string());

	private:
		entt::registry m_Registry;
		friend class Entity;
	};

	class XEN_API Entity
	{
	public:
		Entity() {}
		Entity(Scene* scene) : m_Scene(scene) { m_Entity = m_Scene->m_Registry.create(); }
		~Entity() {}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) { return m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...); }

		template<typename T>
		void DeleteComponent() { m_Scene->m_Registry.remove<T>(m_Entity); }

		template<typename T>
		T& GetComponent() { return m_Scene->m_Registry.get<T>(m_Entity); }

		template<typename... T>
		bool HasAnyComponent() { return m_Scene->m_Registry.any_of<T>(m_Entity); }

		template<typename... T>
		bool HasAllComponent() { return m_Scene->m_Registry.all_of<T>(m_Entity); }

		template<typename T, typename... Args>
		T& ReplaceComponent(Args&&... args) { return m_Scene->m_Registry.replace<T>(std::forward<Args>(args)...); }

	private:
		entt::entity m_Entity;
		Scene* m_Scene;
	};
}

