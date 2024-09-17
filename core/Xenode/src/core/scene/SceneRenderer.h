#pragma once
#include <Core.h>

#include <core/renderer/Camera.h>
#include <core/renderer/FrameBuffer.h>

#include "Scene.h"
#include "SceneRuntime.h"

namespace Xen {

	struct FrameBufferSpec;

	class XEN_API SceneRenderer
	{
	public:
		static void Initialize(uint32_t viewportWidth, uint32_t viewportHeight);

		static void SetActiveScene(const Ref<Scene>& scene);
		static void SetActiveCamera(const Ref<Camera>& camera);

#ifdef XEN_ENABLE_DEBUG_RENDERER
		static void SetSceneDebugSettings(const SceneDebugSettings& debugSettings);
#endif

		static void Update(float timestep, bool isRuntime);
		static void Render(bool renderToGameWindow = false);

		static void End();

		static void ResizeFrameBuffer(uint32_t width, uint32_t height);
		static const Ref<FrameBuffer>& GetActiveFrameBuffer();
	private:
		static void Update2D(float timestep);
		static void Update3D(float timsstep);

		static void Render2D();
		static void Render3D();

#ifdef XEN_ENABLE_DEBUG_RENDERER
		static void UpdateDebugGraphics(float timestep);
		static void RenderDebugGraphics();
#endif
	};
}