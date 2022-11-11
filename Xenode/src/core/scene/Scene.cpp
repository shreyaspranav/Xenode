#include "pch"
#include "Scene.h"

#include "Components.h"
#include "core/app/Log.h"

#include "core/renderer/Renderer2D.h"


namespace Xen {
	void DoSomething(int a)
	{
	}

	Scene::Scene()
	{
		OnCreate();
		
	}

	Scene::~Scene()
	{

	}

	void Scene::OnCreate()
	{

		//quad_entity = m_Registry.create();
		//
		//m_Registry.emplace<Component::Transform>(quad_entity, Vec3(0.0f, 0.0f, 0.0f), 0.0f, Vec2(1.0f));
		//m_Registry.emplace<Component::SpriteRenderer>(quad_entity, Color(1.0f, 0.6f, 0.4f, 1.0f));

#if 0
		entt::registry registry;

		// Creating a entity, adding components to it
		entt::entity quad = registry.create();
		Component::Transform& t = registry.emplace<Component::Transform>(quad, Vec3(1.0f, 1.0f, 0.0f), 0.0f, Vec2(1.0f)); // IMP: You cannot have two same components in a entity
		Component::SpriteRenderer sp = registry.emplace<Component::SpriteRenderer>(quad, Color(1.0f, 0.5f, 0.1f, 1.0f));

		const auto& c_Registry = registry;

		// Getting components from entity
		Component::SpriteRenderer& sprite = registry.get<Component::SpriteRenderer>(quad);
		const Component::SpriteRenderer& c_sprite = c_Registry.get<Component::SpriteRenderer>(quad);

		const auto [csprite, ctransform] = c_Registry.get<Component::SpriteRenderer, Component::Transform>(quad);

		// Replacing a component from a entity
		registry.patch<Component::Transform>(quad, [](auto& transform) {transform.rotation = 1.0f; });
		registry.replace<Component::Transform>(quad, Vec3(1.0f, 1.0f, 0.0f), 0.0f, Vec2(0.5f));

		// When it's unknown whether an entity already owns an instance of a component, emplace_or_replace is the function to use instead
		registry.emplace_or_replace<Component::SpriteRenderer>(quad, Xen::Color(1.0f));

		// To check if a entity has 'all of' the components
		bool all = registry.all_of<Component::Transform, Component::SpriteRenderer>(quad);

		// To check if a entity has 'any of' the components
		bool any = registry.any_of<Component::Transform, Component::SpriteRenderer>(quad);

		// Deleting a component from a entity
		registry.erase<Component::SpriteRenderer>(quad); // Crashes if the component does not exist
		registry.remove<Component::SpriteRenderer>(quad); // Removes if exists

		// Deleting all entities having a specific component
		registry.clear<Component::SpriteRenderer>();

		// Clear all enitities
		registry.clear();

		// Setup a listener when a component is created, destroyed or updated
		registry.on_construct<Component::Transform>().connect<&DoSomething>();
		registry.on_destroy<Component::Transform>().connect<&DoSomething>();
		registry.on_update<Component::Transform>().connect<&DoSomething>();

		// Observers: Way to iterate through a specific entities
		entt::observer update_observer{ registry, entt::collector.update<Component::Transform>() };

		for (const auto& entity : update_observer)
		{
			// iterating all the entites that is updated
		}
		update_observer.clear();

		entt::observer group_observer{ registry, entt::collector.group<Component::Transform, Component::SpriteRenderer>() };

		for (const auto& entity : group_observer)
		{
			// iterating all the entites that is having all the components mentioned (in this case 'Component::Transform' and 'Component::SpriteRenderer')
		}

#endif
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity e = Entity(this);
		e.AddComponent<Xen::Component::Transform>(Xen::Vec3(0.0f, 0.0f, 0.0f), glm::degrees(0.0f), Xen::Vec2(1.0f));
		return e;
	}
	void Scene::OnUpdate(double timestep)
	{
		//entt::observer group_observer{ m_Registry, entt::collector.group<Component::Transform, Component::SpriteRenderer>() };

		auto group_observer = m_Registry.group<Component::Transform, Component::SpriteRenderer>();
		for (const auto& entity : group_observer)
		{
			Component::Transform& transform = m_Registry.get<Component::Transform>(entity);
			Component::SpriteRenderer& spriteRenderer = m_Registry.get<Component::SpriteRenderer>(entity);

			Renderer2D::DrawTexturedQuad(spriteRenderer.texture, transform.position, transform.rotation, transform.scale, spriteRenderer.color);
		}
	}

}