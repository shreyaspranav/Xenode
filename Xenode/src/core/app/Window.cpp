#include "pch"
#include "Window.h"
#include "GameApplication.h"

#include <gfxapi/window/glfw/GLFW_window.h>

namespace Xen {
	Ref<Window> Window::GetWindow(const WindowProps& props)
	{
		GraphicsAPI api = GameApplication::GetGraphicsAPI();

		switch (api)
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<GLFW_window>(props);

		case GraphicsAPI::XEN_VULKAN_API:
			return std::make_shared<GLFW_window>(props);

		case GraphicsAPI::XEN_DIRECT3D_API:
			return nullptr;

		case GraphicsAPI::XEN_OPENGLES_API:
			return nullptr;

		case GraphicsAPI::XEN_METAL_API:
			return nullptr;
		}
	}

	// TODO: Shift to a better timing library.. 
	// This is for just temporary reasons that works only in glfwGetTime()
	double Window::GetTime() { return GLFW_window::GetTime(); } 
}