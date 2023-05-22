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

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto camera_view = m_Registry.view<Component::CameraComp>();

		for (auto& entity : camera_view)
		{
			Component::CameraComp& camera_component = camera_view.get<Component::CameraComp>(entity);

			if (camera_component.is_primary_camera)
				return Entity(entity, this);
		}

		return Entity();
	}

	void Scene::OnUpdateRuntime(double timestep)
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
			Component::Transform& transform = Entity(entity, this).GetComponent<Component::Transform>();
			if (m_RenderableEntities.size() < m_RenderableEntityIndex + 1)
			{
				m_RenderableEntities.push_back(Entity(entity, this));
				m_ZCoordinates.push_back(transform.position.z);
			}
			else {
				m_RenderableEntities[m_RenderableEntityIndex] = Entity(entity, this);

				if (m_ZCoordinates[m_RenderableEntityIndex] != transform.position.z)
					m_IsDirty = true;

				m_ZCoordinates[m_RenderableEntityIndex] = transform.position.z;
			}
			
			m_RenderableEntityIndex++;
		}
		
		for (auto& entity : circle_group_observer)
		{
			Component::Transform& transform = Entity(entity, this).GetComponent<Component::Transform>();
			if (m_RenderableEntities.size() < m_RenderableEntityIndex + 1)
			{
				m_RenderableEntities.push_back(Entity(entity, this));
				m_ZCoordinates.push_back(transform.position.z);
			}
			else {
				m_RenderableEntities[m_RenderableEntityIndex] = Entity(entity, this);

				if (m_ZCoordinates[m_RenderableEntityIndex] != transform.position.z)
					m_IsDirty = true;

				m_ZCoordinates[m_RenderableEntityIndex] = transform.position.z;
			}
			m_RenderableEntityIndex++;
		}


		if(m_IsDirty)
			SortRenderableEntities();
		m_IsDirty = false;

		for (int i = 0; i < m_RenderableEntityIndex; i++)
		{
			Component::Transform& transform = m_RenderableEntities[i].GetComponent<Component::Transform>();
			if (m_RenderableEntities[i].HasAnyComponent<Component::SpriteRenderer>())
			{
				Component::SpriteRenderer& spriteRenderer = m_RenderableEntities[i].GetComponent<Component::SpriteRenderer>();

				if (spriteRenderer.texture == nullptr)
					Renderer2D::DrawClearQuad(transform.position, 
						transform.rotation, 
						{ transform.scale.x, transform.scale.y },
						spriteRenderer.color);
				else
					Renderer2D::DrawTexturedQuad(spriteRenderer.texture, 
						transform.position, 
						transform.rotation, 
						{ transform.scale.x, transform.scale.y },
						spriteRenderer.color,
						spriteRenderer.texture_tile_factor);

			}
			else if (m_RenderableEntities[i].HasAnyComponent<Component::CircleRenderer>())
			{
				Component::CircleRenderer& circleRenderer = m_RenderableEntities[i].GetComponent<Component::CircleRenderer>();

				Renderer2D::DrawClearCircle(transform.position, 
					transform.rotation, 
					{ transform.scale.x, transform.scale.y },
					circleRenderer.color, 
					circleRenderer.thickness, 
					circleRenderer.inner_fade, 
					circleRenderer.outer_fade);
			}
		}
	}

	void Scene::OnUpdate(double timestep, const Ref<Camera>& camera)
	{
		m_RenderableEntityIndex = 0;

		Renderer2D::BeginScene(camera);

		// Render Sprites
		auto sprite_group_observer = m_Registry.view<Component::SpriteRenderer>();
		auto circle_group_observer = m_Registry.view<Component::CircleRenderer>();

		for (auto& entity : sprite_group_observer)
		{
			Component::Transform& transform = Entity(entity, this).GetComponent<Component::Transform>();
			if (m_RenderableEntities.size() < m_RenderableEntityIndex + 1)
			{
				m_RenderableEntities.push_back(Entity(entity, this));
				m_ZCoordinates.push_back(transform.position.z);
			}
			else {
				m_RenderableEntities[m_RenderableEntityIndex] = Entity(entity, this);

				if (m_ZCoordinates[m_RenderableEntityIndex] != transform.position.z)
					m_IsDirty = true;

				m_ZCoordinates[m_RenderableEntityIndex] = transform.position.z;
			}

			m_RenderableEntityIndex++;
		}

		for (auto& entity : circle_group_observer)
		{
			Component::Transform& transform = Entity(entity, this).GetComponent<Component::Transform>();
			if (m_RenderableEntities.size() < m_RenderableEntityIndex + 1)
			{
				m_RenderableEntities.push_back(Entity(entity, this));
				m_ZCoordinates.push_back(transform.position.z);
			}
			else {
				m_RenderableEntities[m_RenderableEntityIndex] = Entity(entity, this);

				if (m_ZCoordinates[m_RenderableEntityIndex] != transform.position.z)
					m_IsDirty = true;

				m_ZCoordinates[m_RenderableEntityIndex] = transform.position.z;
			}
			m_RenderableEntityIndex++;
		}


		if (m_IsDirty)
			SortRenderableEntities();
		m_IsDirty = false;

		for (int i = 0; i < m_RenderableEntityIndex; i++)
		{
			Component::Transform& transform = m_RenderableEntities[i].GetComponent<Component::Transform>();
			if (m_RenderableEntities[i].HasAnyComponent<Component::SpriteRenderer>())
			{
				Component::SpriteRenderer& spriteRenderer = m_RenderableEntities[i].GetComponent<Component::SpriteRenderer>();

				if (spriteRenderer.texture == nullptr) {
					switch (spriteRenderer.primitive)
					{
					case SpriteRendererPrimitive::Triangle:
						Renderer2D::DrawClearTriangle(transform.position,
							transform.rotation,
							{ transform.scale.x, transform.scale.y },
							spriteRenderer.color);
						break;
					case SpriteRendererPrimitive::Quad:
						Renderer2D::DrawClearQuad(transform.position,
							transform.rotation,
							{ transform.scale.x, transform.scale.y },
							spriteRenderer.color);
						break;
					case SpriteRendererPrimitive::Polygon:
						// TODO: Renderer2D Implementation
						Renderer2D::DrawPolygon(transform.position,
							transform.rotation,
							{ transform.scale.x, transform.scale.y },
							spriteRenderer.polygon_segment_count,
							spriteRenderer.color);
						break;
					default:
						break;
					}
				}

				// TODO: Primitive thing for the textured ones:
				else
					Renderer2D::DrawTexturedQuad(spriteRenderer.texture,
						transform.position,
						transform.rotation,
						{ transform.scale.x, transform.scale.y },
						spriteRenderer.color,
						spriteRenderer.texture_tile_factor);

			}
			else if (m_RenderableEntities[i].HasAnyComponent<Component::CircleRenderer>())
			{
				Component::CircleRenderer& circleRenderer = m_RenderableEntities[i].GetComponent<Component::CircleRenderer>();

				Renderer2D::DrawClearCircle(transform.position,
					transform.rotation,
					{ transform.scale.x, transform.scale.y },
					circleRenderer.color,
					circleRenderer.thickness,
					circleRenderer.inner_fade,
					circleRenderer.outer_fade);
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

	void Scene::SortRenderableEntities()
	{
		std::sort(m_RenderableEntities.begin(),
			m_RenderableEntities.begin() + m_RenderableEntityIndex,
			[](const Entity& one, const Entity& another)
			{
				Component::Transform& transform_one = one.GetComponent<Component::Transform>();
				Component::Transform& transform_another = another.GetComponent<Component::Transform>();

				// To avoid Z fighting, make sure that no renderable entities have same z position:
				if (transform_one.position.z == transform_another.position.z)
					transform_one.position.z += 0.001f;

				return transform_one.position.z > transform_another.position.z;
			});

		std::sort(m_ZCoordinates.rbegin(), m_ZCoordinates.rend());
	}
}