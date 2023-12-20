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
		void OnWindowResize(uint32_t width, uint32_t height) override;

		void EnableDepthTest(bool enabled) override;

		void SetBlendMode(BlendMode colorBlendMode, BlendMode alphaBlendMode) override;

		//Draw Commands
		void DrawIndexed(const Ref<VertexBuffer>& vertexArray, uint32_t indices) override;
		void DrawLines(const Ref<VertexBuffer>& vertexArray, uint32_t indices) override;

		void DrawTriangles(const Ref<VertexBuffer>& vertexArray, uint32_t indices) override;

		void SetLineWidth(float width) override;
	};
}
