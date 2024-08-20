#pragma once
#include <Core.h>

#include <core/renderer/Camera.h>
#include <core/renderer/FrameBuffer.h>
#include "Scene.h"

namespace Xen {

	struct FrameBufferSpec;

	class XEN_API SceneRenderer
	{
	public:
		static void Initialize(uint32_t viewportWidth, uint32_t viewportHeight);

		static void SetActiveScene(const Ref<Scene>& scene);
		static void SetActiveCamera(const Ref<Camera>& camera);

		static void Update(double timestep);
		static void Render(bool renderToGameWindow = false);

		static void End();

		static void ResizeFrameBuffer(uint32_t width, uint32_t height);
		static const Ref<FrameBuffer>& GetActiveFrameBuffer();
	private:
		static void Update2D(double timestep);
		static void Update3D(double timsstep);

		static void Render2D();
		static void Render3D();
	};
}