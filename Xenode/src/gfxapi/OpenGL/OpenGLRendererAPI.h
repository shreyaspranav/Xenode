#pragma once

#include <core/renderer/RendererAPI.h>

namespace Xen {
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		OpenGLRendererAPI() {}
		virtual ~OpenGLRendererAPI() {}

		void Clear() override;
		void SetClearColor(const Color& color) override;

		//Draw Commands
		void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
	};
}
