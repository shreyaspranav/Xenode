#include "pch"
#include "OpenGLRendererAPI.h"

#include <glad/gl.h>
#include <core/app/Log.h>

#include <core/app/Profiler.h>

namespace Xen {
	void OpenGLRendererAPI::Clear()
	{
		XEN_PROFILE_FN();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetClearColor(const Color& color)
	{
		XEN_PROFILE_FN();
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRendererAPI::OnWindowResize(uint32_t width, uint32_t height)
	{
		glViewport(0, 0, width, height);
	}
	void OpenGLRendererAPI::SetAdditiveBlendMode(bool b)
	{
		if (b) {
			//glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ONE, GL_ZERO);
			//glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
			glBlendFunc(GL_DST_ALPHA, GL_ONE);
			glBlendEquation(GL_FUNC_ADD);
		}
		else {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
		}

	}
	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indices)
	{
		XEN_PROFILE_FN();
		glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);
	}
	
	void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t indices)
	{
		XEN_PROFILE_FN();
		glDrawArrays(GL_LINES, 0, indices);
	}
	void OpenGLRendererAPI::DrawTriangles(const Ref<VertexArray>& vertexArray, uint32_t indices)
	{
		XEN_PROFILE_FN();
		glDrawArrays(GL_TRIANGLES, 0, indices);
	}
	void OpenGLRendererAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}
}