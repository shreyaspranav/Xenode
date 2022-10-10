#pragma once

#include "RenderCommand.h"

namespace Xen {
	class Renderer2D
	{
	public:
		static void BeginScene();
		static void EndScene();

		static void Submit(const Ref<VertexArray>& vertexArray);
	};
}
