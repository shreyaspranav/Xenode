#pragma once
#include <Core.h>

#ifdef XEN_DEVICE_DESKTOP

#include <core/app/input/MouseInput.h>

namespace Xen
{
	class GLFW_mouse_input
	{
		friend class MouseInput;
	public:
		static bool IsMouseButtonPressed(MouseButtonCode code);
	private:
		static void Init(const Ref<Window>& window);
	};
}

#endif