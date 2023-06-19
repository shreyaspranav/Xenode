#include "pch"
#include "Scene.h"

#include "Components.h"
#include "core/app/Log.h"

#include "core/renderer/Renderer2D.h"
#include "core/app/Log.h"

// Physics:
#include <box2d/box2d.h>
#include "SceneSerializer.h"

constexpr auto DEGTORAD = 0.0174532925199432957f;
constexpr auto RADTODEG = 57.295779513082320876f;

namespace Xen {
	void DoSomething(int a)
	{
	}

	template<typename Comp>
	static void CopyComponentAllEntities(entt::registry& srcSceneRegistry, entt::registry& dstSceneRegistry, const std::unordered_map<UUID, Entity>& uuidMap)
	{
		auto srcRegView = srcSceneRegistry.view<Component::ID>();
		for (entt::entity src_entt : srcRegView)
		{
			UUID uuid = srcSceneRegistry.get<Component::ID>(src_entt).id;
			entt::entity dst_entt = uuidMap.at(uuid);

			if (srcSceneRegistry.any_of<Comp>(src_entt))
			{
				auto& compToBeCopied = srcSceneRegistry.get<Comp>(src_entt);
				dstSceneRegistry.emplace_or_replace<Comp>(dst_entt, compToBeCopied);
			}
		}
	}

	template<typename Comp>
	static void CopyComponent(Entity src, Entity dst)
	{
		if (src.HasAnyComponent<Comp>())
		{
			Comp& toBeCopiedComponent = src.GetComponent<Comp>();
			dst.AddComponent<Comp>(toBeCopiedComponent);
		}
	}

	Scene::Scene()
	{
		OnCreate();
	}

	Scene::~Scene()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::OnCreate()
	{
		m_ScriptEngine = ScriptEngine::InitScriptEngine();

		Ref<Script> script1 = Script::CreateScript("assets/scripts/ScriptOne.lua");
		Ref<Script> script2 = Script::CreateScript("assets/scripts/ScriptTwo.lua");

		m_ScriptEngine->AddScript(script1);
		m_ScriptEngine->AddScript(script2);

		m_ScriptEngine->OnSetup();
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity e = Entity(this);
		e.AddComponent<Component::ID>();
		e.AddComponent<Component::Tag>(name);
		e.AddComponent<Component::Transform>(Xen::Vec3(0.0f), Xen::Vec3(0.0f), Xen::Vec3(1.0f));
		return e;
	}

	Entity Scene::CreateEntityWithUUID(const std::string& name, UUID id)
	{
		Entity e = Entity(this);
		e.AddComponent<Component::ID>(id);
		e.AddComponent<Component::Tag>(name);
		e.AddComponent<Component::Transform>(Xen::Vec3(0.0f), Xen::Vec3(0.0f), Xen::Vec3(1.0f));
		return e;
	}

	Entity Scene::CopyEntity(Entity entity)
	{
		Entity newEntity = Entity(this);
		newEntity.AddComponent<Component::ID>();
		newEntity.AddComponent<Component::Tag>(entity.GetComponent<Component::Tag>().tag);

		CopyComponent<Component::Transform>(entity, newEntity);
		CopyComponent<Component::CameraComp>(entity, newEntity);
		CopyComponent<Component::SpriteRenderer>(entity, newEntity);
		CopyComponent<Component::CircleRenderer>(entity, newEntity);
		CopyComponent<Component::TextRenderer>(entity, newEntity);
		CopyComponent<Component::RigidBody2D>(entity, newEntity);
		CopyComponent<Component::BoxCollider2D>(entity, newEntity);
		CopyComponent<Component::NativeScript>(entity, newEntity);

		return newEntity;
	}

	Entity Scene::GetRuntimeEntity(Entity editorEntity, const Ref<Scene>& runtimeScene)
	{
		if (editorEntity == Entity())
			return Entity();

		if (!editorEntity.IsValid())
			return Entity();
		if (!m_isRunningOnRuntime && editorEntity.IsNull())
			return editorEntity;
		else {
			UUID editorEntityUUID = editorEntity.GetComponent<Component::ID>().id;

			auto runtimeSceneIDView = runtimeScene->m_Registry.view<Component::ID>();
			for (entt::entity e : runtimeSceneIDView)
			{
				Entity entt = Entity(e, runtimeScene.get());
				UUID runtimeEntityUUID = entt.GetComponent<Component::ID>().id;

				if (runtimeEntityUUID == editorEntityUUID)
					return entt;
			}
			XEN_ENGINE_LOG_ERROR("Runtime Entity with the specified UUID is not found!");
			return editorEntity;
		}
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy((entt::entity)entity);
	}

	void Scene::DestroyAllEntities()
	{
		m_Registry.clear();
		m_RenderableEntities.clear();
		m_ZCoordinates.clear();
		m_RenderableEntityIndex = 0;
	}

	void Scene::OnRuntimeStart()
	{
		m_isRunningOnRuntime = true;

		m_PhysicsWorld = new b2World({ 0.0f, -10.0f });
		auto rigid_body_view = m_Registry.view<Component::RigidBody2D>();

		for (auto& entity : rigid_body_view)
		{
			Entity this_entity = Entity(entity, this);

			Component::Transform& transform = this_entity.GetComponent<Component::Transform>();
			Component::RigidBody2D& rigidBody2d = this_entity.GetComponent<Component::RigidBody2D>();

			b2BodyDef bodyDefinition;
			switch (rigidBody2d.bodyType)
			{
			case Component::RigidBody2D::BodyType::Static:
				bodyDefinition.type = b2_staticBody;
				break;
			case Component::RigidBody2D::BodyType::Dynamic:
				bodyDefinition.type = b2_dynamicBody;
				break;
			case Component::RigidBody2D::BodyType::Kinematic:
				bodyDefinition.type = b2_kinematicBody;
				break;
			default:
				break;
			}

			//bodyDefinition.position.Set(transform.position.x, transform.position.y);
			//bodyDefinition.angle = transform.rotation.z;

			b2Body* physicsBody = m_PhysicsWorld->CreateBody(&bodyDefinition);
			physicsBody->SetTransform({transform.position.x, transform.position.y}, transform.rotation.z * DEGTORAD);

			physicsBody->SetFixedRotation(rigidBody2d.fixedRotation);

			// Set the runtime body so that it can deleted later:
			rigidBody2d.runtimeBody = physicsBody;

			if (this_entity.HasAnyComponent<Component::BoxCollider2D>())
			{
				Component::BoxCollider2D& boxCollider = this_entity.GetComponent< Component::BoxCollider2D>();

				b2PolygonShape shape;
				shape.SetAsBox(
					boxCollider.size.x * transform.scale.x,
					boxCollider.size.y * transform.scale.y
				);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &shape;
				fixtureDef.density = boxCollider.bodyDensity;
				fixtureDef.friction = boxCollider.bodyFriction;
				fixtureDef.restitution = boxCollider.bodyRestitution;
				fixtureDef.restitutionThreshold = boxCollider.bodyRestitutionThreshold;

				physicsBody->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;

		m_isRunningOnRuntime = false;
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

	void Scene::NewScene()
	{
		DestroyAllEntities();
		m_IsDirty = true;
	}

	Ref<Scene> Scene::Copy(Ref<Scene> srcScene)
	{
		Ref<Scene> newScene = std::make_shared<Scene>();

		newScene->m_FramebufferWidth = srcScene->m_FramebufferWidth;
		newScene->m_FramebufferHeight = srcScene->m_FramebufferHeight;

		newScene->m_RenderableEntities = srcScene->m_RenderableEntities;
		newScene->m_ZCoordinates = srcScene->m_ZCoordinates;
		newScene->m_RenderableEntityIndex = srcScene->m_RenderableEntityIndex;

		newScene->m_IsDirty = srcScene->m_IsDirty;

		std::unordered_map<UUID, Entity> uuidEntityMap;

		entt::registry& srcSceneRegistry = srcScene->m_Registry;
		entt::registry& dstSceneRegistry = newScene->m_Registry;

		auto entt_view = srcSceneRegistry.view<Component::ID>();

		for (entt::entity e : entt_view)
		{
			Entity entt = Entity(e, srcScene.get());
			Component::ID id = entt.GetComponent<Component::ID>();
			Component::Tag tag = entt.GetComponent<Component::Tag>();

			Entity new_entt = newScene->CreateEntityWithUUID(tag.tag, id.id);

			uuidEntityMap.insert({ id.id, new_entt });

			//uuidEntityMap[id.id] = new_entt;
		}

		CopyComponentAllEntities<Component::Transform>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::CameraComp>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::SpriteRenderer>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::CircleRenderer>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::TextRenderer>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::RigidBody2D>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::BoxCollider2D>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::NativeScript>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);

		return newScene;
	}

	void Scene::OnUpdateRuntime(double timestep, bool paused)
	{
		m_RenderableEntityIndex = 0;
		//entt::observer group_observer{ m_Registry, entt::collector.group<Component::Transform, Component::SpriteRenderer>() };

		if (!paused) {
			UpdateNativeScripts(timestep);
			SimulatePhysics(1.0 / 60.0);
		}

		UpdateCameras();

		RenderSprites();
	}

	void Scene::OnUpdate(double timestep, const Ref<Camera>& camera)
	{
		m_RenderableEntityIndex = 0;

		Renderer2D::BeginScene(camera);

		RenderSprites();
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_FramebufferWidth = width;
		m_FramebufferHeight = height;

		auto camera_group_observer = m_Registry.view<Component::Transform, Component::CameraComp>();
		RenderCommand::OnWindowResize(width, height);
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

				//XEN_ENGINE_LOG_INFO("Sorting: {0} and {1}", one.GetComponent<Component::Tag>().tag, another.GetComponent<Component::Tag>().tag);

				// To avoid Z fighting, make sure that no renderable entities have same z position:
				if (transform_one.position.z == transform_another.position.z)
					transform_one.position.z += 0.001f;

				return transform_one.position.z > transform_another.position.z;
			});

		std::sort(m_ZCoordinates.rbegin(), m_ZCoordinates.rend());
	}
	void Scene::UpdateNativeScripts(double timestep)
	{
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
	}
	void Scene::UpdateCameras()
	{
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
	}
	void Scene::SimulatePhysics(double fixedTimeStep)
	{
		// Simulate Physics:
		const int32_t velocityIterations = 6;
		const int32_t positionIterations = 2;

		m_PhysicsWorld->Step(fixedTimeStep, velocityIterations, positionIterations);

		auto rigid_body_view = m_Registry.view<Component::RigidBody2D>();

		for (auto& entity : rigid_body_view)
		{
			Entity this_entity = Entity(entity, this);

			Component::Transform& transform = this_entity.GetComponent<Component::Transform>();
			Component::RigidBody2D& rigidBody2d = this_entity.GetComponent<Component::RigidBody2D>();

			b2Body* body = (b2Body*)rigidBody2d.runtimeBody;

			const auto& position = body->GetPosition();

			transform.position.x = position.x;
			transform.position.y = position.y;

			transform.rotation.z = (body->GetAngle() * RADTODEG);

			body->SetTransform(body->GetPosition(), body->GetAngle());
		}
	}

	void Scene::RenderSprites()
	{
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
							spriteRenderer.color,
							(uint32_t)m_RenderableEntities[i]);
						break;
					case SpriteRendererPrimitive::Quad:
						Renderer2D::DrawClearQuad(transform.position,
							transform.rotation,
							{ transform.scale.x, transform.scale.y },
							spriteRenderer.color,
							(uint32_t)m_RenderableEntities[i]);
						break;
					case SpriteRendererPrimitive::Polygon:
						Renderer2D::DrawPolygon(transform.position,
							transform.rotation,
							{ transform.scale.x, transform.scale.y },
							spriteRenderer.polygon_segment_count,
							spriteRenderer.color,
							(uint32_t)m_RenderableEntities[i]);
						break;
					default:
						break;
					}
				}
				else
					Renderer2D::DrawTexturedQuad(spriteRenderer.texture,
						transform.position,
						transform.rotation,
						{ transform.scale.x, transform.scale.y },
						spriteRenderer.color,
						spriteRenderer.texture_tile_factor,
						nullptr,
						(uint32_t)m_RenderableEntities[i]);

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
					circleRenderer.outer_fade, 
					(uint32_t)m_RenderableEntities[i]);
			}
		}
	}
}