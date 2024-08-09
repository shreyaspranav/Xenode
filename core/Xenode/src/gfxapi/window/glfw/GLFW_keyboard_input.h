#pragma once
#include <Core.h>

#ifdef XEN_DEVICE_DESKTOP

#include <core/app/input/KeyboardInput.h>

namespace Xen 
{
	class GLFW_keyboard_input
	{
		friend class KeyboardInput;
	public:
		static bool IsKeyPressed(KeyboardKeyCode keyCode);
	private:
		static void Init(const Ref<Window>& window);
	};
}

#endif