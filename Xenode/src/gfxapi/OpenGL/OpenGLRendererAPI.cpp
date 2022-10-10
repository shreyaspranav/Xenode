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
	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetElementBuffer()->GetActiveCount(), GL_UNSIGNED_INT, 0);
	}
}