#include "pch"
#include "SceneRuntime.h"
#include "SceneRenderer.h"
#include "ScenePhysics.h"

#include "Components.h"

#include <core/app/GameApplication.h>
#include <core/app/Log.h>

#include <core/scripting/ScriptEngine.h>


namespace Xen 
{
	// Struct that represents the state of the Scene Runtime:
	struct SceneRuntimeData
	{
		SceneSettings sceneSettings;

		// Specifies if the scene is running or not.
		bool isRunning = false;

		bool paused = false;

		// The pointer to the scene that is currently being handled.
		Ref<Scene> currentScene;

		// The Runtime camera (The camera by which the scene will be rendered during runtime)
		// And its transform component
		Ref<Camera> runtimeCamera;
		Component::Transform runtimeCameraTransform;

		// The Additional camera (The camera by which the scene will be rendered during editing workflow in the editor)
		Ref<Camera> additionalCamera;

		// TEMP:
		// Fixed timestep: (This is to be owned by the Application class)
		const double fixedTimeStep = 1.0 / 60.0;
		
		ScriptEngine* scriptEngine;

	}sceneRuntimeState;


	// Implementation: ----------------------------------------------------------------------------------------------------------------------------
	void SceneRuntime::Initialize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		SceneRenderer::Initialize(viewportWidth, viewportHeight);
		ScenePhysics::Initialize(sceneRuntimeState.fixedTimeStep);
	}

	void SceneRuntime::SetActiveScene(const Ref<Scene>& scene)
	{
		sceneRuntimeState.currentScene = scene;
		SceneRenderer::SetActiveScene(scene);
		ScenePhysics::SetActiveScene(scene);
	}

	void SceneRuntime::SetAdditionalCamera(const Ref<Camera>& camera)
	{
		sceneRuntimeState.additionalCamera = camera;
	}


	void SceneRuntime::Begin(const SceneSettings& sceneSettings)
	{
		sceneRuntimeState.sceneSettings = sceneSettings;

		if (sceneSettings.renderSource == RenderSource::Unknown)
		{
			XEN_ENGINE_LOG_ERROR("The render source is unknown!");
			TRIGGER_BREAKPOINT;
		}

		switch (sceneSettings.renderSource)
		{
		case RenderSource::RuntimeCamera:		SceneRenderer::SetActiveCamera(sceneRuntimeState.runtimeCamera);	break;
		case RenderSource::AdditionalCamera:	SceneRenderer::SetActiveCamera(sceneRuntimeState.additionalCamera);	break;
		}
	}

	void SceneRuntime::RuntimeBegin()
	{
		sceneRuntimeState.isRunning = true;
		ScenePhysics::RuntimeStart(sceneRuntimeState.sceneSettings.gravity);

		// Initialize the script runtime and call OnStart on all scripts
		SceneRuntime::InitScripts();

		// Find the primary camera:
		{
			auto cameraView = sceneRuntimeState.currentScene->m_SceneRegistry.view<Component::CameraComp>();

			for (auto entt : cameraView)
			{
				Entity entity = Entity(entt, sceneRuntimeState.currentScene.get());
				Component::CameraComp& cameraComp = entity.GetComponent<Component::CameraComp>();

				// TODO: Make sure that there is only one primary camera entities:
				if (cameraComp.is_primary_camera)
				{
					sceneRuntimeState.runtimeCameraTransform = entity.GetComponent<Component::Transform>();
					sceneRuntimeState.runtimeCamera = cameraComp.camera;
					SceneRenderer::SetActiveCamera(sceneRuntimeState.runtimeCamera);
					return;
				}
			}
		}

	}
#ifdef XEN_GAME_FINAL_BUILD
	void SceneRuntime::UpdateFinal(double timestep)
	{
		if (!sceneRuntimeState.isRunning) 
		{
			SceneRuntime::RuntimeBegin();
			sceneRuntimeState.isRunning = true;
		}

		SceneRuntime::UpdatePhysics(timestep);
		SceneRuntime::UpdateScripts(timestep);
		SceneRuntime::UpdateSounds(timestep);
		SceneRuntime::UpdateRenderer(timestep);
	}
#else
	void SceneRuntime::Update(double timestep)
	{
		SceneRenderer::Update(timestep);
	}

	void SceneRuntime::UpdateRuntime(double timestep, bool paused)
	{
		sceneRuntimeState.paused = paused;

		if (!sceneRuntimeState.isRunning) 
		{
			XEN_ENGINE_LOG_ERROR("The current scene is not running!");
			TRIGGER_BREAKPOINT;
		}

		if (!paused)
		{
			// Call "OnUpdate" functions on all the scripts:
			{
				auto scriptView = sceneRuntimeState.currentScene->m_SceneRegistry.view<Component::ScriptComp>();

				for (auto entt : scriptView)
				{
					Entity entity = Entity(entt, sceneRuntimeState.currentScene.get());
					Component::ScriptComp& script = entity.GetComponent<Component::ScriptComp>();

					sceneRuntimeState.scriptEngine->OnUpdate(script.script_instance, entity, timestep);
				}
			}

			// Update the runtime camera:
			{
				sceneRuntimeState.runtimeCamera->SetPosition(sceneRuntimeState.runtimeCameraTransform.position);
				sceneRuntimeState.runtimeCamera->SetRotation(sceneRuntimeState.runtimeCameraTransform.rotation);
				sceneRuntimeState.runtimeCamera->SetScale(sceneRuntimeState.runtimeCameraTransform.scale);
				
				sceneRuntimeState.runtimeCamera->Update();
			}
		}
		SceneRenderer::Update(timestep);
	}
#endif // !XEN_GAME_FINAL_BUILD

	void SceneRuntime::FixedUpdate()
	{
		if (sceneRuntimeState.isRunning && !sceneRuntimeState.paused)
		{
			// Call "OnFixedUpdate" functions on all the scripts:
			{
				auto scriptView = sceneRuntimeState.currentScene->m_SceneRegistry.view<Component::ScriptComp>();

				for (auto entt : scriptView)
				{
					Entity entity = Entity(entt, sceneRuntimeState.currentScene.get());
					Component::ScriptComp& script = entity.GetComponent<Component::ScriptComp>();

					sceneRuntimeState.scriptEngine->OnFixedUpdate(script.script_instance, entity);
				}
			}

			// Step the physics
			ScenePhysics::Step(sceneRuntimeState.fixedTimeStep);
		}
	}

	void SceneRuntime::Render()
	{
		SceneRenderer::Render(sceneRuntimeState.sceneSettings.renderToGameWindow);
	}

	void SceneRuntime::RuntimeEnd()
	{
		sceneRuntimeState.isRunning = false;

		delete sceneRuntimeState.scriptEngine;
		ScenePhysics::RuntimeEnd();
	}

	void SceneRuntime::End()
	{
		SceneRenderer::End();
	}

	void SceneRuntime::Finalize()
	{
		if (sceneRuntimeState.isRunning)
		{
			SceneRuntime::RuntimeEnd();
			sceneRuntimeState.isRunning = false;
		}
	}

	void SceneRuntime::ResizeFrameBuffer(uint32_t width, uint32_t height)
	{
		SceneRenderer::ResizeFrameBuffer(width, height);
	}

	const Ref<FrameBuffer> SceneRuntime::GetActiveFrameBuffer()
	{
		return SceneRenderer::GetActiveFrameBuffer();
	}

	// Private functions: ---------------------------------------------------------------------------------------------------------------------------
	void SceneRuntime::InitScripts()
	{
		sceneRuntimeState.scriptEngine = ScriptEngine::InitScriptEngine();

		auto scriptView = sceneRuntimeState.currentScene->m_SceneRegistry.view<Component::ScriptComp>();

		for (auto entt : scriptView)
		{
			Entity entity = Entity(entt, sceneRuntimeState.currentScene.get());
			Component::ScriptComp& script = entity.GetComponent<Component::ScriptComp>();

			sceneRuntimeState.scriptEngine->OnStart(script.script_instance, entity);
		}
	}
}