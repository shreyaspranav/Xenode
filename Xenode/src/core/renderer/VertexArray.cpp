#include "pch"
#include "VertexArray.h"

#include <core/app/GameApplication.h>

#include "gfxapi/OpenGL/OpenGLVertexArray.h"

namespace Xen {
	Ref<VertexArray> VertexArray::GetVertexArray()
	{
		switch (GameApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLVertexArray>();

		case GraphicsAPI::XEN_VULKAN_API:
			return nullptr;
		default:
			break;
		}
	}
}