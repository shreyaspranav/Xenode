#include "pch"
#include "Window.h"

#ifdef XEN_DEVICE_DESKTOP

#include <gfxapi/window/glfw/GLFW_window.h>

namespace Xen {
	Ref<Window> Window::GetWindow(const WindowProps& props)
	{
#ifdef XEN_DEVICE_DESKTOP
		return std::make_shared<GLFW_window>(props);
#elif defined(XEN_DEVICE_MOBILE)
		TRIGGER_BREAKPOINT;
		return nullptr;
#endif
	}
}

#endif