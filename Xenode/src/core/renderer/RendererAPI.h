#pragma once

#include "Structs.h"
#include "VertexArray.h"

namespace Xen {
	class RendererAPI
	{
	public:
		virtual void Clear() = 0;
		virtual void SetClearColor(const Color& color) = 0;
		virtual void OnWindowResize(uint32_t width, uint32_t height) = 0;

		//Draw Commands
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indices) = 0;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t indices) = 0;

		virtual void SetLineWidth(float width) = 0;
	};
}
