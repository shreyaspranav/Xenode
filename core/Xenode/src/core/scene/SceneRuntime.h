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

#ifdef XEN_ENABLE_DEBUG_RENDERER
	enum class DebugRenderTargetFlag : uint8_t
	{
		Disabled	= 0, 
		Editor		= BIT(0),
		Runtime		= BIT(1)
	};

	// Operators for DebugRenderTargetFlag -------------------------------------------------------------------------------
	inline DebugRenderTargetFlag operator&(DebugRenderTargetFlag lhs, DebugRenderTargetFlag rhs)
	{
		return static_cast<DebugRenderTargetFlag>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
	}

	inline DebugRenderTargetFlag operator|(DebugRenderTargetFlag lhs, DebugRenderTargetFlag rhs)
	{
		return static_cast<DebugRenderTargetFlag>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
	}

	inline DebugRenderTargetFlag operator~(DebugRenderTargetFlag flag)
	{
		return static_cast<DebugRenderTargetFlag>(~(static_cast<uint8_t>(flag)));
	}

	inline DebugRenderTargetFlag operator|=(DebugRenderTargetFlag& lhs, DebugRenderTargetFlag&& rhs)
	{
		return lhs = lhs | rhs;
	}

	inline DebugRenderTargetFlag operator&=(DebugRenderTargetFlag& lhs, DebugRenderTargetFlag&& rhs)
	{
		return lhs = lhs & rhs;
	}
	// --------------------------------------------------------------------------------------------------------------------

	struct SceneDebugSettings
	{
		// Global debug rendering flag,
		bool showDebugGraphics = true;

		// Debug rendering flags for various features.
		DebugRenderTargetFlag physicsColliderTargetFlag = DebugRenderTargetFlag::Editor;
		DebugRenderTargetFlag displayEntitiesTargetFlag = DebugRenderTargetFlag::Editor;

		// Color of various stuff
		Color physicsColliderColor		= { 0.0f, 1.0f, 0.0f, 1.0f };
		Color displayEntitiesColor		= { 1.0f, 0.7f, 0.2f, 1.0f };

		bool showFPSOverlay = false;

		// Display entities are those entities which its bounding box needs to be displayed.
		Vector<Entity> displayEntities;
	};
#endif

	enum class RenderSource { Unknown = 0, RuntimeCamera, AdditionalCamera };

	struct SceneSettings 
	{
		Vec3 gravity = { 0.0f, -10.0f, 0.0f };
		RenderSource renderSource = RenderSource::Unknown;

		bool renderToGameWindow = false;

#ifdef XEN_ENABLE_DEBUG_RENDERER
		SceneDebugSettings debugSettings;
#endif
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
		static void UpdateFinal(float timestep);
#else
		static void Update(float timestep);
		static void UpdateRuntime(float timestep, bool paused);
#endif
		static void FixedUpdate();
		
		static void Render();
		
		static void RuntimeEnd();
		static void End();

		static void Finalize();

		static void ResizeFrameBuffer(uint32_t width, uint32_t height);

		static const Ref<FrameBuffer>& GetActiveFrameBuffer();

	private:
		static void InitScripts();
	};
}