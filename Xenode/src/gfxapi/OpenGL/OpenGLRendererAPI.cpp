#include "pch"
#include "OpenGLRendererAPI.h"

#include <glad/gl.h>
#include <core/app/Log.h>


namespace Xen {
	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetClearColor(const Color& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRendererAPI::OnWindowResize(uint32_t width, uint32_t height)
	{
		glViewport(0, 0, width, height);
	}
	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indices)
	{
		if(indices == 0)
			glDrawElements(GL_TRIANGLES, vertexArray->GetElementBuffer()->GetActiveCount(), GL_UNSIGNED_INT, 0);
		else
			glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);
	}
}