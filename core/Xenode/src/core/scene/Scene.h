#pragma once

#include <Core.h>
#include <core/app/UUID.h>
#include <core/asset/Asset.h>

#include <entt.hpp>

class SceneHierarchyPanel;
class ThumbnailGenerator;

namespace Xen 
{
	enum class SceneType { _2D, _2D_AND_3D };
	
	class Entity;

	class XEN_API Scene : public Asset
	{
	public:
		Scene() {}

		Scene(SceneType sceneType);
		~Scene();

		// Creates a new Entity in the Scene and returns it.
		Entity AddNewEntity(const std::string& tag, bool createTransformComponent = true);
		Entity AddNewEntityWithID(const std::string& tag, UUID id, bool createTransformComponent = true);

		// Creates a new child entity for a parent entity and returns it.
		// Not Implemented: Will implement nested entities later.
		Entity AddNewEntity(const Entity& parent, const std::string& tag);

		// Destroys and removes the entity and its children entities from the scene.
		void DestroyEntity(const Entity& entity);

		// Destroys all entities.
		void DestroyAllEntities();

		// Returns the type of the scene.
		SceneType GetSceneType() { return m_SceneType; }

		DEFINE_ASSET_TYPE(AssetType::Scene);

		// Friend classes declared so that these classes can access the entt::registry m_SceneRegistry
		friend class SceneRuntime;
		friend class SceneRenderer;
		friend class ScenePhysics;
		friend class SceneUtils;
		friend class SceneSerializer;
		friend class Entity;

		friend class ::SceneHierarchyPanel;
		friend class ::ThumbnailGenerator;

	private:
		entt::registry m_SceneRegistry;
		SceneType m_SceneType;
	};

	class Entity
	{
	public:
		Entity();
		Entity(Scene* scene);
		Entity(entt::entity e, Scene* scene);

		~Entity();

		bool IsNull();
		bool IsValid();

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_ParentScene->m_SceneRegistry.emplace<T>(m_Entity, std::forward<Args>(args)...);
		}

		template<typename T>
		void DeleteComponent()
		{
			m_ParentScene->m_SceneRegistry.remove<T>(m_Entity);
		}

		template<typename T>
		T& GetComponent()
		{
			return m_ParentScene->m_SceneRegistry.get<T>(m_Entity);
		}

		template<typename... T>
		bool HasAnyComponent()
		{
			return m_ParentScene->m_SceneRegistry.any_of<T...>(m_Entity);
		}

		template<typename... T>
		bool HasAllComponent()
		{
			return m_ParentScene->m_SceneRegistry.all_of<T...>(m_Entity);
		}

		template<typename T, typename... Args>
		T& ReplaceComponent(Args&&... args)
		{
			return m_ParentScene->m_SceneRegistry.replace<T>(std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		T& EmplaceOrReplaceComponent(Args&&... args)
		{
			return m_ParentScene->m_SceneRegistry.emplace_or_replace<T>(m_Entity, std::forward<Args>(args)...);
		}

		operator int32_t() const { return (int32_t)m_Entity; }
		operator entt::entity() const { return m_Entity; }

		bool operator==(const Entity& other) { return other.m_Entity == m_Entity; }
		bool operator!=(const Entity& other) { return other.m_Entity != m_Entity; }

		inline Scene* GetParentScene() { return m_ParentScene; }
	private:
		entt::entity m_Entity;
		Scene* m_ParentScene;
	};
}

