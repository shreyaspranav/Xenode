#include "pch"
#include "Buffer.h"

#include "core/app/DesktopApplication.h"

#include "gfxapi/OpenGL/OpenGLBuffer.h"

namespace Xen {
	Ref<FloatBuffer> FloatBuffer::CreateFloatBuffer(uint32_t count)
	{
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLFloatBuffer>(count);
		}
	}

	Ref<ElementBuffer> ElementBuffer::CreateElementBuffer(uint32_t count)
	{
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLElementBuffer>(count);
		}
	}
}