#include "pch"
#include "RenderCommand.h"

#include <core/app/GameApplication.h>

#include <gfxapi/OpenGL/OpenGLRendererAPI.h>
#include "RendererAPI.h"

namespace Xen 
{
	struct BaseRendererState
	{
		Scope<RendererAPI> rendererAPI;
	}baseRendererState;

	// Implementation: --------------------------------------------------------------------------------------------------------------------
	void RenderCommand::Init()
	{
		switch (GetApplicationInstance()->GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			baseRendererState.rendererAPI = std::make_unique<OpenGLRendererAPI>();
			break;
		default:
			baseRendererState.rendererAPI = nullptr;
			break;
		}
	}
	
	void RenderCommand::Clear()
	{
		baseRendererState.rendererAPI->Clear();
	}

	void RenderCommand::SetClearColor(const Color& color)
	{
		baseRendererState.rendererAPI->SetClearColor(color);
	}

	void RenderCommand::OnWindowResize(uint32_t width, uint32_t height)
	{
		baseRendererState.rendererAPI->OnWindowResize(width, height);
	}

	void RenderCommand::SetBlendMode(BlendMode colorBlendMode, BlendMode alphaBlendMode)
	{
		baseRendererState.rendererAPI->SetBlendMode(colorBlendMode, alphaBlendMode);
	}

	void RenderCommand::EnableDepthTest(bool enabled)
	{
		baseRendererState.rendererAPI->EnableDepthTest(enabled);
	}

	void RenderCommand::EnableRasterizer(bool enabled)
	{
		baseRendererState.rendererAPI->EnableRasterizer(enabled);
	}

	void RenderCommand::DrawIndexed(PrimitiveType type, const Ref<VertexBuffer>& vertexBuffer, int32_t indices)
	{
		baseRendererState.rendererAPI->DrawIndexed(type, vertexBuffer, indices);
	}

	void RenderCommand::DrawNonIndexed(PrimitiveType type, const Ref<VertexBuffer>& vertexBuffer, int32_t indices)
	{
		baseRendererState.rendererAPI->DrawNonIndexed(type, vertexBuffer, indices);
	}

	void RenderCommand::SetLineWidth(float width)
	{
		baseRendererState.rendererAPI->SetLineWidth(width);
	}
}