#pragma once
#include <Core.h>

#include <core/renderer/Structs.h>
#include <core/renderer/Camera.h>
#include <core/renderer/FrameBuffer.h>

#include "Scene.h"
// For Test purposes: see the following comment
// #define XEN_GAME_FINAL_BUILD

namespace Xen 
{
	// Guard this with a preprocessor:
	struct SceneDebugSettings
	{
		// Global Debug Rendering flag
		bool enableDebugRendering;

		// Color of various stuff
		Color physicsColliderColor		= { 0.0f, 1.0f, 0.0f, 1.0f };
		Color rigidBodyColor			= { 0.0f, 1.0f, 0.0f, 1.0f };
	};

	enum class RenderSource { Unknown = 0, RuntimeCamera, AdditionalCamera };

	struct SceneSettings 
	{
		Vec3 gravity = { 0.0f, -10.0f, 0.0f };
		RenderSource renderSource = RenderSource::Unknown;
	};

	// SceneRuntime Class: This class is used to handle the runtime of the scene i.e, Initializing, Updating and 
	// Shutting down of various subsystems that involve into rendering a final scene. 
	// 
	// The functions SceneRuntime::Update() and SceneRuntime::UpdateRuntime() needs to be used in the editor only 
	// therefore they need to be excluded in the final build of the game built by the game engine. The compiler flag
	// XEN_GAME_FINAL_BUILD will be enabled if the build is the final build of the game.

	// Intended use:
	//		Initialize() -> Once
	//		SetActiveScene() -> Once / In a loop
	// 
	//		(Call these functions only if UpdateRuntime() is to be called)
	//		RuntimeBegin() -> Once
	//		RuntimeEnd() -> Once
	// 
	//		Update() / UpdateRuntime() -> In a loop
	//		FixedUpdate() -> In a loop
	//		Render() -> In a loop
	// 
	//		End() -> Once

	class XEN_API SceneRuntime
	{
	public: 
		static void Initialize(uint32_t viewportWidth, uint32_t viewportHeight);

		static void SetActiveScene(const Ref<Scene>& scene);

		// The runtime uses additionalViewportCamera if mentioned(in SceneSettings), to render the scene instead of the primary camera in the scene. 
		static void SetAdditionalCamera(const Ref<Camera>& camera);

		static void Begin(const SceneSettings& sceneSettings);
		static void RuntimeBegin();

#ifdef XEN_GAME_FINAL_BUILD
		static void UpdateFinal(double timestep);
#else
		static void Update(double timestep);
		static void UpdateRuntime(double timestep, bool paused);
#endif
		static void FixedUpdate();
		
		static void Render();
		
		static void RuntimeEnd();
		static void End();

		static void Finalize();

		static void ResizeFrameBuffer(uint32_t width, uint32_t height);

		static const Ref<FrameBuffer> GetActiveFrameBuffer();

		// Method that returns the integer value of a specified pixel in a integer attachment of a framebuffer if an integer framebuffer exists:
		// static int GetIntPixel()
	private:
		static void InitScripts();
	};
}