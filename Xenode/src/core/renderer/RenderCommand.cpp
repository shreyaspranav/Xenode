#include "pch"
#include "RenderCommand.h"

#include "core/app/DesktopApplication.h"

#include "gfxapi/OpenGL/OpenGLRendererAPI.h"
#include "RendererAPI.h"

namespace Xen {

	Scope<RendererAPI> RenderCommand::s_Api = GetRendererAPI();
	Scope<RendererAPI> RenderCommand::GetRendererAPI()
	{
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_unique<OpenGLRendererAPI>();
		}
		return nullptr;
	}

	void RenderCommand::Clear()
	{
		s_Api->Clear();
	}

	void RenderCommand::SetClearColor(const Color& color)
	{
		s_Api->SetClearColor(color);
	}

	void RenderCommand::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_Api->OnWindowResize(width, height);
	}

	void RenderCommand::SetBlendMode(BlendMode colorBlendMode, BlendMode alphaBlendMode)
	{
		s_Api->SetBlendMode(colorBlendMode, alphaBlendMode);
	}

	void RenderCommand::EnableDepthTest(bool enabled)
	{
		s_Api->EnableDepthTest(enabled);
	}

	void RenderCommand::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, uint32_t indices)
	{
		s_Api->DrawIndexed(vertexBuffer, indices);
	}

	void RenderCommand::DrawLines(const Ref<VertexBuffer>& vertexBuffer, uint32_t indices)
	{
		s_Api->DrawLines(vertexBuffer, indices);
	}

	void RenderCommand::DrawTriangles(const Ref<VertexBuffer>& vertexBuffer, uint32_t indices)
	{
		s_Api->DrawTriangles(vertexBuffer, indices);
	}

	void RenderCommand::SetLineWidth(float width)
	{
		s_Api->SetLineWidth(width);
	}
}