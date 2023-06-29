#pragma once

#include <Core.h>
#include <entt.hpp>

#include <core/renderer/Camera.h>
#include <core/app/UUID.h>

#include <scripting/ScriptEngine.h>
#include <core/renderer/FrameBuffer.h>

class SceneHierarchyPanel;
class b2World;

namespace Xen {
	class Entity;

	class XEN_API Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(double timestep, const Ref<Camera>& camera);
		void OnUpdateRuntime(double timestep, bool paused); // if 'paused' = true, scripts and physics won't be updated
		void OnRender();

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnCreate();

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(const std::string& name, UUID id);
		Entity CopyEntity(Entity entity);
		Entity GetRuntimeEntity(Entity editorEntity, const Ref<Scene>& runtimeScene);

		const Ref<FrameBuffer>& GetSceneFrameBuffer();
		const Ref<FrameBuffer>& GetUnlitSceneFrameBuffer();

		inline uint8_t GetMousePickingFrameBufferIndex() { return 2; }

		void DestroyEntity(Entity entity);
		void DestroyAllEntities();

		Entity GetPrimaryCameraEntity();

		void NewScene();

		static Ref<Scene> Copy(Ref<Scene> srcScene);
	private:
		void SortRenderableEntities();

		void UpdateNativeScripts(double timestep);
		void UpdateScripts(double timestep);
		void UpdateCameras();
		void SimulatePhysics(double fixedTimeStep);
		void RenderSprites();
		void RenderLights();

	private:
		entt::registry m_Registry;
		uint32_t m_FramebufferWidth = 1, m_FramebufferHeight = 1;

		std::vector<Entity> m_RenderableEntities;
		std::vector<float> m_ZCoordinates;
		uint32_t m_RenderableEntityIndex = 0;

		bool m_IsDirty = true;
		bool m_isRunningOnRuntime = false;

		b2World* m_PhysicsWorld = nullptr;

		Ref<ScriptEngine> m_ScriptEngine;

		// FrameBuffers:
		Ref<FrameBuffer> m_UnlitSceneFB;
		Ref<FrameBuffer> m_LightMaskFB;
		Ref<FrameBuffer> m_FinalSceneFB;

		friend class Entity;
		friend class ::SceneHierarchyPanel;
		friend class SceneSerializer;
	};

	class XEN_API Entity
	{
	public:
		Entity() : m_Entity(entt::null), m_Scene(nullptr) {}
		Entity(Scene* scene) : m_Scene(scene) { m_Entity = m_Scene->m_Registry.create(); }
		Entity(entt::entity e, Scene* scene) : m_Entity(e), m_Scene(scene) {}
		~Entity() {}

		template<typename T, typename... Args>
		inline T& AddComponent(Args&&... args) const { return m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...); }

		template<typename T>
		inline void DeleteComponent() const { m_Scene->m_Registry.remove<T>(m_Entity); }

		template<typename T>
		inline T& GetComponent() const { return m_Scene->m_Registry.get<T>(m_Entity); }

		template<typename... T>
		inline bool HasAnyComponent() const { return m_Scene->m_Registry.any_of<T...>(m_Entity); }

		template<typename... T>
		inline bool HasAllComponent() const { return m_Scene->m_Registry.all_of<T...>(m_Entity); }

		template<typename T, typename... Args>
		inline T& ReplaceComponent(Args&&... args) const { return m_Scene->m_Registry.replace<T>(std::forward<Args>(args)...); }

		operator uint32_t() const { return (uint32_t)m_Entity; }
		operator entt::entity() const { return m_Entity; }

		bool IsNull() { return m_Entity == entt::null ? true : false; }
		bool IsValid() { return m_Scene->m_Registry.valid(m_Entity); }
		bool operator==(const Entity& other) { return other.m_Entity == m_Entity; }
		bool operator!=(const Entity& other) { return other.m_Entity != m_Entity; }

	private:
		entt::entity m_Entity = entt::null;
		Scene* m_Scene;
	};
}

