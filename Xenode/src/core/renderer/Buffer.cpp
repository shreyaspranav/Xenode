#include "pch"
#include "Buffer.h"

#include "core/app/GameApplication.h"

#include "gfxapi/OpenGL/OpenGLBuffer.h"

namespace Xen {
	Ref<FloatBuffer> FloatBuffer::CreateFloatBuffer(uint32_t count)
	{
		switch (GameApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLFloatBuffer>(count);
		case GraphicsAPI::XEN_VULKAN_API:
			return nullptr;
		}
	}

	Ref<ElementBuffer> ElementBuffer::CreateElementBuffer(uint32_t count)
	{
		switch (GameApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLElementBuffer>(count);
		case GraphicsAPI::XEN_VULKAN_API:
			return nullptr;
		}
	}
}