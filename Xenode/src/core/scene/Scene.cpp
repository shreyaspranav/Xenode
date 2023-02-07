#include "pch"
#include "Scene.h"

#include "Components.h"
#include "core/app/Log.h"

#include "core/renderer/Renderer2D.h"
#include "core/app/Log.h"

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
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity e = Entity(this);
		e.AddComponent<Xen::Component::Transform>(Xen::Vec3(0.0f), Xen::Vec3(0.0f), Xen::Vec3(1.0f));
		e.AddComponent<Xen::Component::Tag>(name);
		return e;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy((entt::entity)entity);
	}

	void Scene::DestroyAllEntities()
	{
		m_Registry.clear();
	}

	void Scene::OnUpdate(double timestep)
	{
		m_RenderableEntityIndex = 0;
		//entt::observer group_observer{ m_Registry, entt::collector.group<Component::Transform, Component::SpriteRenderer>() };

		// Run Native Scripts
		m_Registry.view<Component::NativeScript>().each([=](auto entity, Component::NativeScript& script) 
			{
				if (!script.scriptable_entity_instance)
				{
					script.scriptable_entity_instance = script.InstantiateScript();
					script.scriptable_entity_instance->OnCreate();
					script.scriptable_entity_instance->m_Entity = Entity(entity, this);
				}

				script.scriptable_entity_instance->OnUpdate(timestep);
			});

		// Update Cameras
		auto camera_group_observer = m_Registry.view<Component::Transform, Component::CameraComp>();
		for (auto& entity : camera_group_observer)
		{
			Component::Transform& transform = camera_group_observer.get<Component::Transform>(entity);
			Component::CameraComp& camera = camera_group_observer.get<Component::CameraComp>(entity);

			if (camera.is_primary_camera)
			{
				camera.camera->SetPosition(transform.position);
				camera.camera->SetRotation(transform.rotation);
				camera.camera->SetScale(transform.scale);
				camera.camera->Update();
				Renderer2D::BeginScene(camera.camera);
			}
		}

		// Render Sprites
		auto sprite_group_observer = m_Registry.view<Component::SpriteRenderer>();
		auto circle_group_observer = m_Registry.view<Component::CircleRenderer>();

		for (auto& entity : sprite_group_observer)
		{	
			if (m_RenderableEntities.size() < m_RenderableEntityIndex + 1)
				m_RenderableEntities.push_back(Entity(entity, this));
			else 
				m_RenderableEntities[m_RenderableEntityIndex] = Entity(entity, this);
			
			m_RenderableEntityIndex++;
		}
		
		for (auto& entity : circle_group_observer)
		{
			if (m_RenderableEntities.size() < m_RenderableEntityIndex + 1)
				m_RenderableEntities.push_back(Entity(entity, this));
			else
				m_RenderableEntities[m_RenderableEntityIndex] = Entity(entity, this);
			m_RenderableEntityIndex++;
		}

		// Sort the vector of renderable entities by passing in a lambda of how to tell one is less than another
		std::sort(m_RenderableEntities.begin(), m_RenderableEntities.end(), [](const Entity& one, const Entity& another)
			{
				Component::Transform& transform_one = one.GetComponent<Component::Transform>();
				Component::Transform& transform_another = another.GetComponent<Component::Transform>();

				return transform_one.position.z < transform_another.position.z;
			});

		for (Entity& e : m_RenderableEntities)
		{
			if (e.HasAnyComponent<Component::SpriteRenderer>())
			{
				Component::Transform& transform = e.GetComponent<Component::Transform>();
				Component::SpriteRenderer& spriteRenderer = e.GetComponent<Component::SpriteRenderer>();

				if (spriteRenderer.texture == nullptr)
					Renderer2D::DrawClearQuad(transform.position, transform.rotation, transform.scale, spriteRenderer.color);
				else
					Renderer2D::DrawTexturedQuad(spriteRenderer.texture, transform.position, transform.rotation, transform.scale, spriteRenderer.color);
			}
			else if (e.HasAnyComponent<Component::CircleRenderer>())
			{
				Component::Transform& transform = e.GetComponent<Component::Transform>();
				Component::CircleRenderer& circleRenderer = e.GetComponent<Component::CircleRenderer>();

				Renderer2D::DrawClearCircle(transform.position, transform.rotation, transform.scale, circleRenderer.color, circleRenderer.thickness, circleRenderer.inner_fade, circleRenderer.outer_fade);
			}
		}
	}
	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_FramebufferWidth = width;
		m_FramebufferHeight = height;

		auto camera_group_observer = m_Registry.view<Component::Transform, Component::CameraComp>();
		//RenderCommand::OnWindowResize(width, height);
		for (auto& entity : camera_group_observer)
		{
			Component::CameraComp& camera = camera_group_observer.get<Component::CameraComp>(entity);

			if (camera.is_primary_camera && camera.is_resizable)
			{
				camera.camera->OnViewportResize(width, height);
				camera.camera->Update();
			}
		}
	}
}