#include "pch"
#include "Window.h"
#include "DesktopApplication.h"

#include <gfxapi/window/glfw/GLFW_window.h>

namespace Xen {
	Ref<Window> Window::GetWindow(const WindowProps& props)
	{
		GraphicsAPI api = DesktopApplication::GetGraphicsAPI();

		switch (api)
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<GLFW_window>(props);
		}
		return nullptr;
	}
}