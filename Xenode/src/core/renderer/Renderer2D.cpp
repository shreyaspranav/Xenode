#include "pch"
#include "Renderer2D.h"

namespace Xen {
	void Renderer2D::BeginScene()
	{
	}
	void Renderer2D::EndScene()
	{
	}
	void Renderer2D::Submit(const Ref<VertexArray>& vertexArray)
	{
		RenderCommand::DrawIndexed(vertexArray);
	}
}