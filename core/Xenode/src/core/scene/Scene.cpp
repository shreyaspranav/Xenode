#include "pch"
#include "Scene.h"
#include "Components.h"

namespace Xen {
	Entity::Entity()
	{
		m_Entity = entt::null;
		m_ParentScene = nullptr;
	}

	Entity::Entity(Scene* scene)
		:m_ParentScene(scene)
	{
		m_Entity = scene->m_SceneRegistry.create();
	}

	Entity::Entity(entt::entity e, Scene* scene)
		:m_Entity(e), m_ParentScene(scene)
	{}

	Entity::~Entity()
	{

	}

	bool Entity::IsNull()
	{
		return m_Entity == entt::null;
	}

	bool Entity::IsValid()
	{
		return m_ParentScene->m_SceneRegistry.valid(m_Entity);
	}

	// Scene class implementation:
	Scene::Scene(SceneType sceneType)
		:m_SceneType(sceneType)
	{

	}
	Scene::~Scene()
	{
		Scene::DestroyAllEntities();
	}

	Entity Scene::AddNewEntity(const std::string& tag, bool createTransformComponent)
	{
		Entity entt = Entity(this);

		entt.AddComponent<Component::ID>();
		entt.AddComponent<Component::Tag>(tag);

		if(createTransformComponent)
			entt.AddComponent<Component::Transform>();

		return entt;
	}

	Entity Scene::AddNewEntityWithID(const std::string& tag, UUID id, bool createTransformComponent)
	{
		Entity entt = Entity(this);
	
		entt.AddComponent<Component::ID>(id);
		entt.AddComponent<Component::Tag>(tag);
	
		if(createTransformComponent)
			entt.AddComponent<Component::Transform>();
	
		return entt;
	}

	Entity Scene::AddNewEntity(const Entity& parent, const std::string& tag)
	{
		XEN_ENGINE_LOG_ERROR_SEVERE("Entity Scene::AddNewEntity(const Entity& parent, const std::string& tag) Not Implemented!");
		TRIGGER_BREAKPOINT;

		return Scene::AddNewEntity(tag);
	}

	void Scene::DestroyEntity(const Entity& entity)
	{
		m_SceneRegistry.destroy(entity);
	}

	void Scene::DestroyAllEntities()
	{
		m_SceneRegistry.clear();
	}

}