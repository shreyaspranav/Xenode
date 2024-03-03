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


		void SetBlendMode(BlendMode colorBlendMode, BlendMode alphaBlendMode) override;
		void EnableDepthTest(bool enabled) override;
		void EnableRasterizer(bool enabled) override;

		//Draw Commands
		void DrawIndexed(PrimitiveType type, const Ref<VertexBuffer>& vertexArray, int32_t indices) override;
		void DrawNonIndexed(PrimitiveType type, const Ref<VertexBuffer>& vertexArray, int32_t vertices) override;

		void SetLineWidth(float width) override;
	};
}
