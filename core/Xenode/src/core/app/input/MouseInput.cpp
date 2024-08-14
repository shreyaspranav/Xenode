#include "pch"
#include "MouseInput.h"

#include <gfxapi/window/glfw/GLFW_mouse_input.h>

namespace Xen
{
	bool MouseInput::IsMouseButtonPressed(MouseButtonCode code)
	{
#ifdef XEN_DEVICE_DESKTOP
		return GLFW_mouse_input::IsMouseButtonPressed(code);
#elif defined(XEN_DEVICE_MOBILE)
		TRIGGER_BREAKPOINT;
#endif
	}

	MousePointer MouseInput::GetMousePointer()
	{
#ifdef XEN_DEVICE_DESKTOP
		return GLFW_mouse_input::GetMousePointer();
#elif defined(XEN_DEVICE_MOBILE)
		TRIGGER_BREAKPOINT;
#endif
	}

	// Private Functions: -------------------------------------------------------------------------------------------------
	void MouseInput::Init(const Ref<Window>& window)
	{
#ifdef XEN_DEVICE_DESKTOP
		GLFW_mouse_input::Init(window);
#elif defined(XEN_DEVICE_MOBILE)
		TRIGGER_BREAKPOINT;
#endif
	}
}

std::ostream& operator<<(std::ostream& os, Xen::MouseButtonAction action)
{
	os << (uint32_t)action;
	return os;
}

std::ostream& operator<<(std::ostream& os, Xen::MouseButtonCode code)
{
	os << (uint16_t)code;
	return os;
}
