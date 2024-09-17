#pragma once

#include<Core.h>

namespace Xen
{
	class Camera;

	class XEN_API SpriteRenderer2D
	{
	private:
		friend class Renderer2D;

		static void Init();
		static void BeginScene(const Ref<Camera>& camera);
		static void EndScene();

		static void RenderFrame(float timestep);
	};
}