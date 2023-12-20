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

		static void SetBlendMode(BlendMode colorBlendMode, BlendMode alphaBlendMode);
		static void EnableDepthTest(bool enabled);

		// Draw Commands:
		static void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, uint32_t indices = 0);
		static void DrawLines(const Ref<VertexBuffer>& vertexBuffer, uint32_t indices = 0);
		static void DrawTriangles(const Ref<VertexBuffer>& vertexBuffer, uint32_t indices = 0);

		static void SetLineWidth(float width);

	private:
		static Scope<RendererAPI> GetRendererAPI();
		static Scope<RendererAPI> s_Api;
	};
}
