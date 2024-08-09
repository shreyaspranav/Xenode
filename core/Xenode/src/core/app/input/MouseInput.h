#pragma once
#include <Core.h>

namespace Xen
{
	class Window;

	enum class MouseButtonAction : uint8_t { Release = 0, Press = 1, Hold = 2 };

	enum class MouseButtonCode : uint16_t
	{
		MOUSE_BUTTON_1 = 0,
		MOUSE_BUTTON_2 = 1,
		MOUSE_BUTTON_3 = 2,
		MOUSE_BUTTON_4 = 3,
		MOUSE_BUTTON_5 = 4,
		MOUSE_BUTTON_6 = 5,
		MOUSE_BUTTON_7 = 6,
		MOUSE_BUTTON_8 = 7,
		MOUSE_BUTTON_LAST = MOUSE_BUTTON_8,
		MOUSE_BUTTON_LEFT = MOUSE_BUTTON_1,
		MOUSE_BUTTON_RIGHT = MOUSE_BUTTON_2,
		MOUSE_BUTTON_MIDDLE = MOUSE_BUTTON_3,
	};

	class XEN_API MouseInput
	{
		friend class DesktopGameApplication;
	public:
		// Get the status of the Mouse Button
		static bool IsMouseButtonPressed(MouseButtonCode code);

		// These set of methods should only be called by children of GameApplication
	private:
		// Sets up all the events that can be raised by mouse and sets the "active window" to window
		static void Init(const Ref<Window>& window);
	};
}

std::ostream& operator<<(std::ostream& os, Xen::MouseButtonAction action);
std::ostream& operator<<(std::ostream& os, Xen::MouseButtonCode code);
