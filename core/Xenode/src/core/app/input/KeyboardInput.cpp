#include "pch"
#include "KeyboardInput.h"

#ifdef XEN_DEVICE_DESKTOP
#include <gfxapi/window/glfw/GLFW_keyboard_input.h>
#endif

namespace Xen
{
	bool KeyboardInput::IsKeyPressed(KeyboardKeyCode code)
	{
#ifdef XEN_DEVICE_DESKTOP
		// return GLFW implementation
		return GLFW_keyboard_input::IsKeyPressed(code);
#elif defined(XEN_DEVICE_MOBILE)
		// return other implementations
		TRIGGER_BREAKPOINT;
		return KeyAction();
#endif
	}

	void KeyboardInput::Init(const Ref<Window>& window)
	{
#ifdef XEN_DEVICE_DESKTOP
		// GLFW implementation
		GLFW_keyboard_input::Init(window);
#elif defined(XEN_DEVICE_MOBILE)
		// other implementations
		TRIGGER_BREAKPOINT;
		return KeyAction();
#endif
	}
}

std::ostream& operator<<(std::ostream& os, Xen::KeyboardKeyCode code)
{
	os << (uint16_t)code;
	return os;
}

std::ostream& operator<<(std::ostream& os, Xen::KeyAction code)
{
	os << (uint32_t)code;
	return os;
}
