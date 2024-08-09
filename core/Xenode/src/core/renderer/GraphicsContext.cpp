#include "pch"
#include "GraphicsContext.h"

#include <core/app/GameApplication.h>

#include <gfxapi/OpenGL/OpenGLContext.h>

namespace Xen {
	GraphicsContext* GraphicsContext::CreateContext(const Ref<Window>& window)
	{
		switch (GetApplicationInstance()->GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return new OpenGLContext((GLFWwindow*)window->GetNativeWindow());
		}
		return nullptr;
	}
}