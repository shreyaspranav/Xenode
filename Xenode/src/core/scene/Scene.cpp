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

	void Scene::OnUpdate(double timestep)
	{
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
				Renderer2D::BeginScene(camera.camera, Vec2(1.0f, 1.0f));
				camera.camera->Update();
			}
		}

		// Render Sprites
		auto sprite_group_observer = m_Registry.view<Component::Transform, Component::SpriteRenderer>();
		for (auto& entity : sprite_group_observer)
		{
			Component::Transform& transform = sprite_group_observer.get<Component::Transform>(entity);
			Component::SpriteRenderer& spriteRenderer = sprite_group_observer.get<Component::SpriteRenderer>(entity);
			
			if(spriteRenderer.texture == nullptr)
				Renderer2D::DrawClearQuad(transform.position, transform.rotation, transform.scale, spriteRenderer.color);
			else
				Renderer2D::DrawTexturedQuad(spriteRenderer.texture, transform.position, transform.rotation, transform.scale, spriteRenderer.color);
		}
	}
	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		auto camera_group_observer = m_Registry.view<Component::Transform, Component::CameraComp>();
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