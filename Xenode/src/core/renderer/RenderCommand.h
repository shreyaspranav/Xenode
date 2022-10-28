#pragma once

#include <Core.h>
#include "RendererAPI.h"

namespace Xen {
	class XEN_API RenderCommand
	{
	public:
		static void Clear();
		static void SetClearColor(const Color& color);
		static void OnWindowResize(uint32_t width, uint32_t height);

		// Draw Commands:
		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indices = 0);

	private:
		static Scope<RendererAPI> GetRendererAPI();
		static Scope<RendererAPI> s_Api;
	};
}
