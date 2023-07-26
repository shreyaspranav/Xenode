#include "pch"
#include "Buffer.h"

#include "core/app/DesktopApplication.h"

#include "gfxapi/OpenGL/OpenGLBuffer.h"

namespace Xen {
	Ref<VertexBuffer> VertexBuffer::CreateVertexBuffer(Size size, const VertexBufferLayout& layout)
	{
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLVertexBuffer>(size, layout);
		}
		return nullptr;
	}

	Ref<ElementBuffer> ElementBuffer::CreateElementBuffer(Size size, ElementBufferDataType dataType)
	{
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLElementBuffer>(size, dataType);
		}
		return nullptr;
	}
}