#include "pch"
#include "FrameBuffer.h"

#include <core/app/GameApplication.h>

#include <gfxapi/OpenGL/OpenGLFrameBuffer.h>

namespace Xen {

	Ref<FrameBuffer> FrameBuffer::CreateFrameBuffer(const FrameBufferSpec& spec)
	{
		switch (GetApplicationInstance()->GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLFrameBuffer>(spec);
		}
		return nullptr;
	}
}