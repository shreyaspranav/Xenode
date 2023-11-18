#include "pch"
#include "Scene.h"

#include "Components.h"
#include "core/app/Log.h"

#include "core/renderer/Renderer2D.h"
#include "core/app/Log.h"
#include "core/renderer/ScreenRenderer.h"

#include <glad/gl.h>

#include "core/app/Timer.h"

// Physics:
#include <box2d/box2d.h>
#include "SceneSerializer.h"

constexpr auto DEGTORAD = 0.0174532925199432957f;
constexpr auto RADTODEG = 57.295779513082320876f;

namespace Xen {
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

		FrameBufferSpec unlit_fb_specs;
		unlit_fb_specs.width = DesktopApplication::GetWindow()->GetWidth();
		unlit_fb_specs.height = DesktopApplication::GetWindow()->GetHeight();


		// TODO FIX: When samples is more than 1, some weird texture atlas shows up instead of the scene
		unlit_fb_specs.samples = 1;

		// Unlit Scene FrameBuffer configuration:
		FrameBufferAttachmentSpec main_layer;
		main_layer.format = FrameBufferTextureFormat::R11G11B10F;
		main_layer.clearColor = Color(0.0f, 0.0f, 0.0f, 0.0f);
		main_layer.filtering = FrameBufferFiltering::Linear;
		//main_layer.mipmaps = 10;

		FrameBufferAttachmentSpec mask_layer;
		mask_layer.format = FrameBufferTextureFormat::RGB8;
		mask_layer.clearColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
		mask_layer.filtering = FrameBufferFiltering::Linear;

		FrameBufferAttachmentSpec mouse_picking_layer;
		mouse_picking_layer.format = FrameBufferTextureFormat::RI;
		mouse_picking_layer.clearColor = Color(-1.0f, 0.0f, 0.0f, 1.0f);

		// LightMask FrameBuffer configuration:
		FrameBufferSpec lightmask_specs;
		lightmask_specs.width = DesktopApplication::GetWindow()->GetWidth();
		lightmask_specs.height = DesktopApplication::GetWindow()->GetHeight();

		lightmask_specs.samples = 1;

		FrameBufferAttachmentSpec light_layer;
		light_layer.format = FrameBufferTextureFormat::R11G11B10F;
		//light_layer.clearColor = Xen::Color(glm::sqrt(0.2f), glm::sqrt(0.2f), glm::sqrt(0.2f), 1.0f);
		light_layer.clearColor = Color(0.0f, 0.0f, 0.0f, 1.0f);

		// Final Scene FrameBuffer configuration:
		FrameBufferSpec final_scene_specs;
		final_scene_specs.width = DesktopApplication::GetWindow()->GetWidth();
		final_scene_specs.height = DesktopApplication::GetWindow()->GetHeight();

		final_scene_specs.samples = 1;

		FrameBufferAttachmentSpec final_layer;
		final_layer.format = FrameBufferTextureFormat::R11G11B10F;
		//light_layer.clearColor = Xen::Color(glm::sqrt(0.2f), glm::sqrt(0.2f), glm::sqrt(0.2f), 1.0f);
		final_layer.clearColor = Color(0.0f, 0.0f, 0.0f, 0.0f);
		final_layer.filtering = FrameBufferFiltering::Linear;

		unlit_fb_specs.attachments = { main_layer, mask_layer, mouse_picking_layer,
			FrameBufferTextureFormat::Depth24_Stencil8 };

		lightmask_specs.attachments = { light_layer };

		final_scene_specs.attachments = { final_layer };

		m_UnlitSceneFB = FrameBuffer::CreateFrameBuffer(unlit_fb_specs);
		m_LightMaskFB = FrameBuffer::CreateFrameBuffer(lightmask_specs);
		m_FinalSceneFB = FrameBuffer::CreateFrameBuffer(final_scene_specs);

		ScreenRenderer2D::Init();

		m_BloomProperties.intensity = 1.0f;
		m_BloomProperties.threshold = 1.0f;
		
		//Ref<BloomEffect> bloomEffect = std::make_shared<BloomEffect>(&m_BloomProperties);
		
		//PostProcessPipeline::AddPostEffects({ bloomEffect });
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
		CopyComponent<Component::AmbientLight>(entity, newEntity);

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

				Vec3 newPosition = { transform.position.x + boxCollider.bodyOffset.x, transform.position.y + boxCollider.bodyOffset.y, transform.position.z };
				physicsBody->SetTransform({ newPosition.x, newPosition.y }, transform.rotation.z * DEGTORAD);

				rigidBody2d.runtimeBody = physicsBody;

				b2PolygonShape shape;
				shape.SetAsBox(
					boxCollider.sizeScale.x * transform.scale.x * 0.5f,
					boxCollider.sizeScale.y * transform.scale.y * 0.5f
				);

				b2FixtureDef fixtureDef;

				fixtureDef.shape = &shape;
				fixtureDef.density = rigidBody2d.bodyDensity;
				fixtureDef.friction = rigidBody2d.bodyFriction;
				fixtureDef.restitution = rigidBody2d.bodyRestitution;
				fixtureDef.restitutionThreshold = rigidBody2d.bodyRestitutionThreshold;

				physicsBody->CreateFixture(&fixtureDef);
			}

			if (this_entity.HasAnyComponent<Component::CircleCollider2D>())
			{
				Component::CircleCollider2D& circleCollider = this_entity.GetComponent<Component::CircleCollider2D>();
			
				Vec3 newPosition = { transform.position.x + circleCollider.bodyOffset.x, transform.position.y + circleCollider.bodyOffset.y, transform.position.z };
				physicsBody->SetTransform({ newPosition.x, newPosition.y }, transform.rotation.z * DEGTORAD);

				rigidBody2d.runtimeBody = physicsBody;

				b2CircleShape circleShape;
				circleShape.m_p.SetZero();
				circleShape.m_radius = circleCollider.radiusScale * transform.scale.x * 0.5f;

				b2FixtureDef fixtureDef;

				fixtureDef.shape = &circleShape;
				fixtureDef.density = rigidBody2d.bodyDensity;
				fixtureDef.friction = rigidBody2d.bodyFriction;
				fixtureDef.restitution = rigidBody2d.bodyRestitution;
				fixtureDef.restitutionThreshold = rigidBody2d.bodyRestitutionThreshold;

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

	void Scene::Test()
	{
		//m_BloomShader = ComputeShader::CreateComputeShader("assets/shaders/bloom.shader");
		//m_BloomShader->LoadShader();
		//
		//m_TestInputTexture = Texture2D::CreateTexture2D("assets/textures/opengl.png", true);
		//m_TestInputTexture->LoadTexture();
		//
		//TextureProperties p = m_TestInputTexture->GetTextureProperties();
		//
		//m_TestOutputTexture = Texture2D::CreateTexture2D({ m_FramebufferWidth / 2, m_FramebufferHeight / 2, TextureFormat::RGBA16F, 0 }, nullptr, 0);
		//m_TestOutputTexture2 = Texture2D::CreateTexture2D({ m_FramebufferWidth / 4, m_FramebufferWidth / 4, TextureFormat::RGBA16F, 0 }, nullptr, 0);
		//m_TestOutputTexture3 = Texture2D::CreateTexture2D({ p.width / 8, p.height / 8, TextureFormat::RGBA8, 0 }, nullptr, 0);
		//m_TestOutputTexture4 = Texture2D::CreateTexture2D({ p.width / 16, p.height / 16, TextureFormat::RGBA8, 0 }, nullptr, 0);
		//m_TestOutputTexture5 = Texture2D::CreateTexture2D({p.width / 32, p.height / 32, TextureFormat::RGBA8, 0}, nullptr, 0);
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

		newScene->m_ShowPhysicsColliders = srcScene->m_ShowPhysicsColliders;
		newScene->m_ShowPhysicsCollidersRuntime = srcScene->m_ShowPhysicsCollidersRuntime;

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
		CopyComponentAllEntities<Component::CircleCollider2D>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::NativeScript>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::ScriptComp>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::PointLight>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::AmbientLight>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);

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

			SimulatePhysics(timestep / 1000.0);
		}

		UpdateCameras();

		RenderSprites();

		if (m_ShowPhysicsCollidersRuntime)
			RenderPhysicsColliders();
		//RenderLights();
	}

	void Scene::OnUpdate(double timestep, const Ref<Camera>& camera)
	{
		m_RenderableEntityIndex = 0;

		Renderer2D::BeginScene(camera);

		RenderSprites();

		if(m_ShowPhysicsColliders)
			RenderPhysicsColliders();
		//RenderLights();
	}

	void Scene::OnRender()
	{
		RenderCommand::Clear();
		m_UnlitSceneFB->ClearAttachments();

		Renderer2D::EndScene();

		RenderCommand::SetBlendMode(
			{ BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOperation::Add },
			{ BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOperation::Add }
		);
		Renderer2D::RenderFrame();
		Renderer2D::RenderOverlay();

		m_UnlitSceneFB->Unbind();

		//PostProcessPipeline::ProcessPostEffects(m_UnlitSceneFB, m_LightMaskFB, { true });


		//m_FinalSceneFB->Bind();
		//RenderCommand::Clear();
		//m_FinalSceneFB->ClearAttachments();
		//ScreenRenderer2D::RenderFinalSceneToScreen(m_UnlitSceneFB->GetColorAttachmentRendererID(0), m_UnlitSceneFB->GetColorAttachmentRendererID(1), m_LightMaskFB->GetColorAttachmentRendererID(0));
		//m_FinalSceneFB->Unbind();


		// Disabling all the light related stuff until I fix the shader situation.

#if 0
		m_LightMaskFB->Bind();
		RenderCommand::Clear();
		m_LightMaskFB->ClearAttachments();
		
		RenderLights();
		
		m_LightMaskFB->Unbind();
		
		m_FinalSceneFB->Bind();
		RenderCommand::Clear();
		m_FinalSceneFB->ClearAttachments();
		
		RenderCommand::SetBlendMode(
			{ BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOperation::Add },
			{ BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOperation::Add }
		);
		//ScreenRenderer2D::RenderTextureToScreen(nullptr);
		ScreenRenderer2D::RenderFinalSceneToScreen(m_UnlitSceneFB->GetColorAttachmentRendererID(0), m_UnlitSceneFB->GetColorAttachmentRendererID(1), m_LightMaskFB->GetColorAttachmentRendererID(0));
		m_FinalSceneFB->Unbind();
#endif
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

		PostProcessPipeline::OnFrameBufferResize(width, height);
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

				return transform_one.position.z < transform_another.position.z;
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

		int physicsStepIterations = 2;

		for(int i = 0; i < physicsStepIterations; i++)
			m_PhysicsWorld->Step(fixedTimeStep, velocityIterations, positionIterations);

		auto rigid_body_view = m_Registry.view<Component::RigidBody2D>();

		for (auto& entity : rigid_body_view)
		{
			Entity this_entity = Entity(entity, this);

			Component::Transform& transform = this_entity.GetComponent<Component::Transform>();
			Component::RigidBody2D& rigidBody2d = this_entity.GetComponent<Component::RigidBody2D>();
			
			Vec2 offset;
			if (this_entity.HasAnyComponent<Component::BoxCollider2D>())
			{
				Component::BoxCollider2D& boxCollider2d = this_entity.GetComponent<Component::BoxCollider2D>();
				offset = boxCollider2d.bodyOffset;
			}
			else if (this_entity.HasAnyComponent<Component::CircleCollider2D>())
			{
				Component::CircleCollider2D& circleCollider2D = this_entity.GetComponent<Component::CircleCollider2D>();
				offset = circleCollider2D.bodyOffset;
			}
			b2Body* body = (b2Body*)rigidBody2d.runtimeBody;

			const auto& position = body->GetPosition();

			transform.position.x = position.x - offset.x;
			transform.position.y = position.y - offset.y;

			transform.rotation.z = (body->GetAngle() * RADTODEG);

			//body->SetTransform(body->GetPosition(), body->GetAngle());
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
					//Renderer2D::DrawClearTriangle(transform.position,
					//	transform.rotation,
					//	{ transform.scale.x, transform.scale.y },
					//	spriteRenderer.color,
					//	(uint32_t)entity);
					break;
				case Component::SpriteRenderer::Primitive::Quad:
					Renderer2D::DrawClearQuad(transform.position,
						transform.rotation,
						{ transform.scale.x, transform.scale.y },
						spriteRenderer.color,
						(uint32_t)entity);
					break;
				case Component::SpriteRenderer::Primitive::Polygon:
					//Renderer2D::DrawPolygon(transform.position,
					//	transform.rotation,
					//	{ transform.scale.x, transform.scale.y },
					//	spriteRenderer.polygon_properties.segment_count,
					//	spriteRenderer.color,
					//	(uint32_t)entity);
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

		// TODO: Make RenderableLayers, that can be arranged in order and can be rendered in order
		// This sorting of renderable entities kind of buggy, hence disabled.

		if (true) {
			m_Registry.sort<Component::SpriteRenderer>([&](const entt::entity& lhs, const entt::entity& rhs)
				{
					Component::Transform& lhsTransform = m_Registry.get<Component::Transform>(lhs);
					Component::Transform& rhsTransform = m_Registry.get<Component::Transform>(rhs);
			
					return lhsTransform.position.z > rhsTransform.position.z;
				});
			//SortRenderableEntities();
		}
		m_IsDirty = false;
	}
	void Scene::RenderPhysicsColliders()
	{
		auto boxColliders = m_Registry.view<Component::BoxCollider2D>();
		auto circleColliders = m_Registry.view<Component::CircleCollider2D>();

		for (auto& entity : boxColliders)
		{
			Component::BoxCollider2D collider = Entity(entity, this).GetComponent<Component::BoxCollider2D>();
			Component::Transform transform = Entity(entity, this).GetComponent<Component::Transform>();

			Vec3 newPosition = { transform.position.x + collider.bodyOffset.x, transform.position.y + collider.bodyOffset.y, transform.position.z };
			Vec2 newScale = { transform.scale.x * collider.sizeScale.x, transform.scale.y * collider.sizeScale.y};

			Renderer2D::DrawQuadOverlay(newPosition, transform.rotation, newScale, m_PhysicsColliderColor);
		}

		for (auto& entity : circleColliders)
		{
			Component::CircleCollider2D collider = Entity(entity, this).GetComponent<Component::CircleCollider2D>();
			Component::Transform transform = Entity(entity, this).GetComponent<Component::Transform>();

			Vec3 newPosition = { transform.position.x + collider.bodyOffset.x, transform.position.y + collider.bodyOffset.y, transform.position.z };
			float scale = collider.radiusScale * transform.scale.x;

			Renderer2D::DrawCircleOverlay(newPosition, scale, m_PhysicsColliderColor);
		}
	}
	void Scene::RenderLights()
	{
		m_LightMaskFB->SetClearColor(0, { 1.0f, 1.0f, 1.0f, 1.0f });

		auto point_light_view = m_Registry.view<Component::PointLight>();
		auto ambient_light_view = m_Registry.view<Component::AmbientLight>();

		for (const entt::entity& e : ambient_light_view)
		{
			Entity entt = Entity(e, this);
			Component::AmbientLight& light = entt.GetComponent<Component::AmbientLight>();
			
			// Disable Lights for now. Will revisit later.
			//m_LightMaskFB->SetClearColor(0, light.color * light.intensity);
		}

		for (const entt::entity& e : point_light_view)
		{
			Entity entt = Entity(e, this);
			Component::Transform& transform = entt.GetComponent<Component::Transform>();
			Component::PointLight& light = entt.GetComponent<Component::PointLight>();
			
			// Disable Lights for now. Will revisit later.
			//Renderer2D::PointLight(transform.position, light.lightColor, light.radius, light.intensity, light.fallofA, light.fallofB);
		}
	}
}