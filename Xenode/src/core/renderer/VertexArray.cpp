#include "pch"
#include "VertexArray.h"

#include <core/app/DesktopApplication.h>

#include "gfxapi/OpenGL/OpenGLVertexArray.h"

namespace Xen {
	Ref<VertexArray> VertexArray::CreateVertexArray()
	{
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLVertexArray>();
		default:
			break;
		}
		return nullptr;
	}
}