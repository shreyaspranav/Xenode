#pragma once

#include "Color.h"
#include "VertexArray.h"

namespace Xen {
	class RendererAPI
	{
	public:
		virtual void Clear() = 0;
		virtual void SetClearColor(const Color& color) = 0;

		//Draw Commands
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;
	};
}
