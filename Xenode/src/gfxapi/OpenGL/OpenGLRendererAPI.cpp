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

		XEN_ENGINE_LOG_ERROR("Invalid BlendFactor!");
		TRIGGER_BREAKPOINT;

		return -1;
	}

	static int ToGLBlendOperation(BlendOperation operation)
	{
		switch (operation)
		{
			case BlendOperation::Add:				return GL_FUNC_ADD;
			case BlendOperation::Subtract:			return GL_FUNC_SUBTRACT;
			case BlendOperation::ReverseSubtract:	return GL_FUNC_REVERSE_SUBTRACT;
		}

		XEN_ENGINE_LOG_ERROR("Invalid BlendOperation!");
		TRIGGER_BREAKPOINT;

		return -1;
	}

	static int ToGLDrawPrimitive(PrimitiveType type)
	{
		switch (type)
		{
			case PrimitiveType::Points:			return GL_POINTS;
			case PrimitiveType::Lines:			return GL_LINES;
			case PrimitiveType::LineLoop:		return GL_LINE_LOOP;
			case PrimitiveType::LineStrip:		return GL_LINE_STRIP;
			case PrimitiveType::Triangles:		return GL_TRIANGLES;
			case PrimitiveType::TriangleFan:	return GL_TRIANGLE_FAN;
			case PrimitiveType::TriangleStrip:	return GL_TRIANGLE_STRIP;
		}

		XEN_ENGINE_LOG_ERROR("Invalid PrimitiveType!");
		TRIGGER_BREAKPOINT;

		return -1;
	}

	static int ToGLElementBufferDataType(ElementBufferDataType type)
	{
		switch (type)
		{
			case Xen::ElementBufferDataType::Unsigned16Bit: return GL_UNSIGNED_SHORT;
			case Xen::ElementBufferDataType::Unsigned32Bit: return GL_UNSIGNED_INT;
		}

		XEN_ENGINE_LOG_ERROR("Invalid ElementBufferDataType!");
		TRIGGER_BREAKPOINT;

		return -1;
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
	void OpenGLRendererAPI::EnableDepthTest(bool enabled)
	{
		if (enabled)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}
	void OpenGLRendererAPI::EnableRasterizer(bool enabled)
	{
		enabled ? glDisable(GL_RASTERIZER_DISCARD) : glEnable(GL_RASTERIZER_DISCARD);
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
	void OpenGLRendererAPI::DrawIndexed(PrimitiveType type, const Ref<VertexBuffer>& vertexBuffer, int32_t indices)
	{
		XEN_PROFILE_FN();

		Ref<ElementBuffer> elementBuffer = vertexBuffer->GetElementBuffer();

		indices = (indices < 0) ? elementBuffer->GetCount() : indices;

		if (vertexBuffer)
			vertexBuffer->Bind();

		glDrawElements(ToGLDrawPrimitive(type), 
			indices, 
			ToGLElementBufferDataType(elementBuffer->GetElementBufferDataType()), 
			0);
	}
	
	void OpenGLRendererAPI::DrawNonIndexed(PrimitiveType type, const Ref<VertexBuffer>& vertexBuffer, int32_t vertices)
	{
		XEN_PROFILE_FN();

		vertices = (vertices < 0) ? vertexBuffer->GetCount() : vertices;

		if(vertexBuffer)
			vertexBuffer->Bind();

		glDrawArrays(ToGLDrawPrimitive(type), 
			0, 
			vertices);
	}
	void OpenGLRendererAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}
}