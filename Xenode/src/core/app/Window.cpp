#include "pch"
#include "Window.h"
#include "GameApplication.h"

#include <gfxapi/window/glfw/GLFW_window.h>

namespace Xen {
	Scope<Window> Window::GetWindow(const WindowProps& props)
	{
		GraphicsAPI api = GameApplication::GetGraphicsAPI();

		switch (api)
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_unique<GLFW_window>(props);

		case GraphicsAPI::XEN_VULKAN_API:
			return nullptr;

		case GraphicsAPI::XEN_DIRECT3D_API:
			return nullptr;

		case GraphicsAPI::XEN_OPENGLES_API:
			return nullptr;

		case GraphicsAPI::XEN_METAL_API:
			return nullptr;
		}
	}
}