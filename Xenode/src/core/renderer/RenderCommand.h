#pragma once

#include <Core.h>
#include "RendererAPI.h"

namespace Xen {
	class XEN_API RenderCommand
	{
	public:
		static void Clear();
		static void SetClearColor(const Color& color);

		// Draw Commands:
		static void DrawIndexed(const Ref<VertexArray>& vertexArray);

	private:
		static Scope<RendererAPI> GetRendererAPI();
		static Scope<RendererAPI> s_Api;
	};
}
