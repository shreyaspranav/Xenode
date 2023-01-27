#include "pch"
#include "GraphicsContext.h"
#include <core/app/DesktopApplication.h>

#include "gfxapi/OpenGL/OpenGLContext.h"

namespace Xen {
	GraphicsContext* GraphicsContext::CreateContext(const Ref<Window>& window)
	{
		GraphicsAPI api = DesktopApplication::GetGraphicsAPI();

		switch (api)
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return new OpenGLContext((GLFWwindow*)window->GetNativeWindow());

		case GraphicsAPI::XEN_VULKAN_API:
			//return new VulkanContext(window);

		case GraphicsAPI::XEN_DIRECT3D_API:
			return nullptr;

		case GraphicsAPI::XEN_OPENGLES_API:
			return nullptr;

		case GraphicsAPI::XEN_METAL_API:
			return nullptr;
		}
	}
}