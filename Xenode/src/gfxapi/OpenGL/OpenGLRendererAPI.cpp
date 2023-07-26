#include "pch"
#include "OpenGLRendererAPI.h"

#include <glad/gl.h>
#include <core/app/Log.h>

#include <core/app/Profiler.h>
#include <core/renderer/RenderCommand.h>

namespace Xen {

	static int ToGLBlendFactor(BlendFactor factor)
	{
		switch (factor)
		{
			case BlendFactor::Zero:					return GL_ZERO;
			case BlendFactor::One:					return GL_ONE;
			case BlendFactor::DstColor:				return GL_DST_COLOR;
			case BlendFactor::OneMinusDstColor:		return GL_ONE_MINUS_DST_COLOR;
			case BlendFactor::DstAlpha:				return GL_DST_ALPHA;
			case BlendFactor::OneMinusDstAlpha:		return GL_ONE_MINUS_DST_ALPHA;
			case BlendFactor::SrcColor:				return GL_SRC_COLOR;
			case BlendFactor::OneMinusSrcColor:		return GL_ONE_MINUS_SRC_COLOR;
			case BlendFactor::SrcAlpha:				return GL_SRC_ALPHA;
			case BlendFactor::OneMinusSrcAlpha:		return GL_ONE_MINUS_SRC_ALPHA;
		}
	}

	static int ToGLBlendOperation(BlendOperation operation)
	{
		switch (operation)
		{
		case Xen::Add:				return GL_FUNC_ADD;
		case Xen::Subtract:			return GL_FUNC_SUBTRACT;
		case Xen::ReverseSubtract:	return GL_FUNC_REVERSE_SUBTRACT;
		}
	}

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
	void OpenGLRendererAPI::SetBlendMode(BlendMode colorBlendMode, BlendMode alphaBlendMode)
	{
		glBlendFuncSeparate(
			ToGLBlendFactor(colorBlendMode.srcFactor),
			ToGLBlendFactor(colorBlendMode.dstFactor),
			ToGLBlendFactor(alphaBlendMode.srcFactor),
			ToGLBlendFactor(alphaBlendMode.dstFactor)
		);

		glBlendEquationSeparate(
			ToGLBlendOperation(colorBlendMode.blendOperation),
			ToGLBlendOperation(alphaBlendMode.blendOperation)
		);
	}
	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, uint32_t indices)
	{
		XEN_PROFILE_FN();
		glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);
	}
	
	void OpenGLRendererAPI::DrawLines(const Ref<VertexBuffer>& vertexBuffer, uint32_t indices)
	{
		XEN_PROFILE_FN();
		glDrawArrays(GL_LINES, 0, indices);
	}
	void OpenGLRendererAPI::DrawTriangles(const Ref<VertexBuffer>& vertexBuffer, uint32_t indices)
	{
		XEN_PROFILE_FN();
		glDrawArrays(GL_TRIANGLES, 0, indices);
	}
	void OpenGLRendererAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}
}