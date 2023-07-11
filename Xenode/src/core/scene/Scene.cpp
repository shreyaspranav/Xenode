#include "pch"
#include "Scene.h"

#include "Components.h"
#include "core/app/Log.h"

#include "core/renderer/Renderer2D.h"
#include "core/app/Log.h"
#include "core/renderer/ScreenRenderer.h"

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

		Xen::FrameBufferSpec unlit_fb_specs;
		unlit_fb_specs.width = Xen::DesktopApplication::GetWindow()->GetWidth();
		unlit_fb_specs.height = Xen::DesktopApplication::GetWindow()->GetHeight();

		// TODO FIX: When samples is more than 1, some weird texture atlas shows up instead of the scene
		unlit_fb_specs.samples = 1;

		// Unlit Scene FrameBuffer configuration:
		Xen::FrameBufferAttachmentSpec main_layer;
		main_layer.format = Xen::FrameBufferTextureFormat::RGB16F;
		main_layer.clearColor = Xen::Color(0.0f, 0.0f, 0.0f, 1.0f);

		Xen::FrameBufferAttachmentSpec mask_layer;
		mask_layer.format = Xen::FrameBufferTextureFormat::RGB8;
		mask_layer.clearColor = Xen::Color(0.0f, 0.0f, 0.0f, 1.0f);

		Xen::FrameBufferAttachmentSpec mouse_picking_layer;
		mouse_picking_layer.format = Xen::FrameBufferTextureFormat::RI;
		mouse_picking_layer.clearColor = Xen::Color(-1.0f, 0.0f, 0.0f, 1.0f);

		// LightMask FrameBuffer configuration:
		Xen::FrameBufferSpec lightmask_specs;
		lightmask_specs.width = Xen::DesktopApplication::GetWindow()->GetWidth();
		lightmask_specs.height = Xen::DesktopApplication::GetWindow()->GetHeight();

		lightmask_specs.samples = 1;

		Xen::FrameBufferAttachmentSpec light_layer;
		light_layer.format = Xen::FrameBufferTextureFormat::RGB16F;
		//light_layer.clearColor = Xen::Color(glm::sqrt(0.2f), glm::sqrt(0.2f), glm::sqrt(0.2f), 1.0f);
		light_layer.clearColor = Xen::Color(0.0f, 0.0f, 0.0f, 1.0f);

		// Final Scene FrameBuffer configuration:
		Xen::FrameBufferSpec final_scene_specs;
		final_scene_specs.width = Xen::DesktopApplication::GetWindow()->GetWidth();
		final_scene_specs.height = Xen::DesktopApplication::GetWindow()->GetHeight();

		final_scene_specs.samples = 1;

		Xen::FrameBufferAttachmentSpec final_layer;
		final_layer.format = Xen::FrameBufferTextureFormat::RGB16F;
		//light_layer.clearColor = Xen::Color(glm::sqrt(0.2f), glm::sqrt(0.2f), glm::sqrt(0.2f), 1.0f);
		final_layer.clearColor = Xen::Color(0.0f, 0.0f, 0.0f, 1.0f);

		unlit_fb_specs.attachments = { main_layer, mask_layer, mouse_picking_layer,
			Xen::FrameBufferTextureFormat::Depth24_Stencil8 };

		lightmask_specs.attachments = { light_layer };

		final_scene_specs.attachments = { final_layer };

		m_UnlitSceneFB = FrameBuffer::CreateFrameBuffer(unlit_fb_specs);
		m_LightMaskFB = FrameBuffer::CreateFrameBuffer(lightmask_specs);
		m_FinalSceneFB = FrameBuffer::CreateFrameBuffer(final_scene_specs);

		ScreenRenderer2D::Init();
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
		CopyComponent<Component::TextRenderer>(entity, newEntity);
		CopyComponent<Component::RigidBody2D>(entity, newEntity);
		CopyComponent<Component::BoxCollider2D>(entity, newEntity);
		CopyComponent<Component::NativeScript>(entity, newEntity);
		CopyComponent<Component::ScriptComp>(entity, newEntity);
		CopyComponent<Component::PointLight>(entity, newEntity);

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

	const Ref<FrameBuffer>& Scene::GetSceneFrameBuffer()
	{
		return m_FinalSceneFB;
	}

	const Ref<FrameBuffer>& Scene::GetUnlitSceneFrameBuffer()
	{
		return m_UnlitSceneFB;
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
		auto view = m_Registry.view<Component::ScriptComp>();

		for (auto& e : view)
		{
			Entity entt = Entity(e, this);

			Component::ScriptComp& scriptComp = entt.GetComponent<Component::ScriptComp>();
			m_ScriptEngine->OnStart(scriptComp.script_instance);
		}

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
		CopyComponentAllEntities<Component::SpriteRenderer>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);;
		CopyComponentAllEntities<Component::TextRenderer>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::RigidBody2D>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::BoxCollider2D>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::NativeScript>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::ScriptComp>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::PointLight>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);

		return newScene;
	}

	void Scene::OnUpdateRuntime(double timestep, bool paused)
	{
		m_RenderableEntityIndex = 0;
		//entt::observer group_observer{ m_Registry, entt::collector.group<Component::Transform, Component::SpriteRenderer>() };

		if (!paused) {
			// C++ Scripts:
			UpdateNativeScripts(timestep);

			// Lua/C# Scripts:
			UpdateScripts(timestep);

			SimulatePhysics(1.0 / 60.0);
		}

		UpdateCameras();

		RenderSprites();
		RenderLights();
	}

	void Scene::OnUpdate(double timestep, const Ref<Camera>& camera)
	{
		m_RenderableEntityIndex = 0;

		Renderer2D::BeginScene(camera);

		RenderSprites();
		RenderLights();
	}

	void Scene::OnRender()
	{
		RenderCommand::Clear();
		m_UnlitSceneFB->ClearAttachments();

		Renderer2D::EndScene();

		RenderCommand::SetAdditiveBlendMode(false);
		Renderer2D::RenderFrame();

		m_UnlitSceneFB->Unbind();

		m_LightMaskFB->Bind();
		RenderCommand::Clear();
		m_LightMaskFB->ClearAttachments();

		RenderCommand::SetAdditiveBlendMode(true);
		Renderer2D::RenderLights();
		m_LightMaskFB->Unbind();

		//Ref<Texture2D> unlitSceneTexture = Texture2D::CreateTexture2D(m_UnlitSceneFB->GetColorAttachmentRendererID(0));
		//Ref<Texture2D> lightMapTexture = Texture2D::CreateTexture2D(m_LightMaskFB->GetColorAttachmentRendererID(0));

		m_FinalSceneFB->Bind();
		RenderCommand::Clear();
		m_FinalSceneFB->ClearAttachments();

		RenderCommand::SetAdditiveBlendMode(false);
		//ScreenRenderer2D::RenderTextureToScreen(nullptr);
		ScreenRenderer2D::RenderFinalSceneToScreen(m_UnlitSceneFB->GetColorAttachmentRendererID(0), m_UnlitSceneFB->GetColorAttachmentRendererID(1), m_LightMaskFB->GetColorAttachmentRendererID(0));
		m_FinalSceneFB->Unbind();
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

		m_UnlitSceneFB->Resize(width, height);
		m_LightMaskFB->Resize(width, height);
		m_FinalSceneFB->Resize(width, height);
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
	void Scene::UpdateScripts(double timestep)
	{
		// Run Scripts(Lua/C#)

		auto view = m_Registry.view<Component::ScriptComp>();

		for (auto& e : view)
		{
			Entity entt = Entity(e, this);

			Component::ScriptComp& scriptComp = entt.GetComponent<Component::ScriptComp>();
			m_ScriptEngine->OnUpdate(scriptComp.script_instance, timestep);
		}
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
		m_UnlitSceneFB->Bind();

		// Render Sprites
		auto sprite_group_observer = m_Registry.view<Component::SpriteRenderer>();

		for (auto& entity : sprite_group_observer)
		{
			Component::Transform& transform = Entity(entity, this).GetComponent<Component::Transform>();

			Component::SpriteRenderer& spriteRenderer = Entity(entity, this).GetComponent<Component::SpriteRenderer>();

			if (spriteRenderer.texture == nullptr) {
				switch (spriteRenderer.primitive)
				{
				case Component::SpriteRenderer::Primitive::Triangle:
					Renderer2D::DrawClearTriangle(transform.position,
						transform.rotation,
						{ transform.scale.x, transform.scale.y },
						spriteRenderer.color,
						(uint32_t)entity);
					break;
				case Component::SpriteRenderer::Primitive::Quad:
					Renderer2D::DrawClearQuad(transform.position,
						transform.rotation,
						{ transform.scale.x, transform.scale.y },
						spriteRenderer.color,
						(uint32_t)entity);
					break;
				case Component::SpriteRenderer::Primitive::Polygon:
					Renderer2D::DrawPolygon(transform.position,
						transform.rotation,
						{ transform.scale.x, transform.scale.y },
						spriteRenderer.polygon_properties.segment_count,
						spriteRenderer.color,
						(uint32_t)entity);
					break;
				case Component::SpriteRenderer::Primitive::Circle:
					Renderer2D::DrawClearCircle(transform.position,
						transform.rotation,
						{ transform.scale.x, transform.scale.y },
						spriteRenderer.color,
						spriteRenderer.circle_properties.thickness,
						spriteRenderer.circle_properties.innerfade,
						spriteRenderer.circle_properties.outerfade,
						(uint32_t)entity);
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
					(uint32_t)entity);

		}
#if 0
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
#endif

		// TODO: Make RenderableLayers, that can be arranged in order and can be rendered in order
		// This sorting of renderable entities kind of buggy, hence disabled.

		if (true) {
			m_Registry.sort<Component::SpriteRenderer>([&](const entt::entity& lhs, const entt::entity& rhs)
				{
					Component::Transform& lhsTransform = m_Registry.get<Component::Transform>(lhs);
					Component::Transform& rhsTransform = m_Registry.get<Component::Transform>(rhs);
			
					return lhsTransform.position.z > rhsTransform.position.z;
				});
			//
			//m_Registry.sort<Component::CircleRenderer>([&](const entt::entity& lhs, const entt::entity& rhs)
			//	{
			//		Component::Transform& lhsTransform = m_Registry.get<Component::Transform>(lhs);
			//		Component::Transform& rhsTransform = m_Registry.get<Component::Transform>(rhs);
			//
			//		return lhsTransform.position.z > rhsTransform.position.z;
			//	});
			SortRenderableEntities();
		}
		m_IsDirty = false;
#if 0
		for (int i = 0; i < m_RenderableEntityIndex; i++)
		{
			Component::Transform& transform = m_RenderableEntities[i].GetComponent<Component::Transform>();
			if (m_RenderableEntities[i].HasAnyComponent<Component::SpriteRenderer>())
			{
				Component::SpriteRenderer& spriteRenderer = m_RenderableEntities[i].GetComponent<Component::SpriteRenderer>();

				if (spriteRenderer.texture == nullptr) {
					switch (spriteRenderer.primitive)
					{
					case Component::SpriteRenderer::Primitive::Triangle:
						Renderer2D::DrawClearTriangle(transform.position,
							transform.rotation,
							{ transform.scale.x, transform.scale.y },
							spriteRenderer.color,
							(uint32_t)m_RenderableEntities[i]);
						break;
					case Component::SpriteRenderer::Primitive::Quad:
						Renderer2D::DrawClearQuad(transform.position,
							transform.rotation,
							{ transform.scale.x, transform.scale.y },
							spriteRenderer.color,
							(uint32_t)m_RenderableEntities[i]);
						break;
					case Component::SpriteRenderer::Primitive::Polygon:
						Renderer2D::DrawPolygon(transform.position,
							transform.rotation,
							{ transform.scale.x, transform.scale.y },
							spriteRenderer.polygon_segment_count,
							spriteRenderer.color,
							(uint32_t)m_RenderableEntities[i]);
						break;
					case Component::SpriteRenderer::Primitive::Circle:
						Renderer2D::DrawClearCircle(transform.position,
							transform.rotation,
							{ transform.scale.x, transform.scale.y },
							spriteRenderer.color,
							spriteRenderer.circle_properties.thickness,
							spriteRenderer.circle_properties.innerfade,
							spriteRenderer.circle_properties.outerfade,
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
		
#endif
	}
	void Scene::RenderLights()
	{
		auto view = m_Registry.view<Component::PointLight>();

		for (const entt::entity& e : view)
		{
			Entity entt = Entity(e, this);
			Component::Transform& transform = entt.GetComponent<Component::Transform>();
			Component::PointLight& light = entt.GetComponent<Component::PointLight>();
			
			Renderer2D::PointLight(transform.position, light.lightColor, light.radius, light.fallofA, light.fallofB);
		}
	}
}