#include "pch"
#include "GLFW_mouse_input.h"

#ifdef XEN_DEVICE_DESKTOP
#include "GLFW_window.h"

#include <core/app/Events.h>

#include <GLFW/glfw3.h>

namespace Xen
{
	struct GLFW_mouse_input_state
	{
		GLFWwindow* currentWindow;
	}mouseInputState;
	
	// Private Functions: ---------------------------------------------------------------------------------------------------
	static MouseButtonAction ToMouseButtonAction(int action)
	{
		switch (action)
		{
		case GLFW_PRESS:	return MouseButtonAction::Press;
		case GLFW_RELEASE:	return MouseButtonAction::Release;
		}
	}

	// Implementation: ------------------------------------------------------------------------------------------------------
	bool GLFW_mouse_input::IsMouseButtonPressed(MouseButtonCode code)
	{
		// Change here if MouseButtonCode is changed
		int mouseButtonCode = (int)code;

		return glfwGetMouseButton(mouseInputState.currentWindow, mouseButtonCode) == GLFW_PRESS ? true : false;
	}

	MousePointer GLFW_mouse_input::GetMousePointer()
	{
		double x, y;
		glfwGetCursorPos(mouseInputState.currentWindow, &x, &y);

		return MousePointer((uint16_t)x, (uint16_t)y);
	}

	void GLFW_mouse_input::Init(const Ref<Window>& window)
	{
		// Set the window in the current state.
		// TODO: Make sure that 'window' is a GLFW window
		mouseInputState.currentWindow = (GLFWwindow*)window->GetNativeWindow();

		glfwSetMouseButtonCallback(mouseInputState.currentWindow,
			[](GLFWwindow* window, int button, int action, int mods) 
			{
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
				MouseButtonEvent buttonEvent((MouseButtonCode)button, ToMouseButtonAction(action));

				data->eventCallbackFn(buttonEvent);
			});

		glfwSetCursorPosCallback(mouseInputState.currentWindow,
			[](GLFWwindow* window, double xpos, double ypos)
			{
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
				MouseMoveEvent moveEvent((uint16_t)xpos, (uint16_t)ypos);

				data->eventCallbackFn(moveEvent);
			});

		glfwSetCursorEnterCallback(mouseInputState.currentWindow,
			[](GLFWwindow* window, int entered)
			{
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
				MouseEnterEvent enterEvent((bool)entered);

				data->eventCallbackFn(enterEvent);
			});
		
		glfwSetScrollCallback(mouseInputState.currentWindow,
			[](GLFWwindow* window, double xoffset, double yoffset) 
			{
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
				MouseScrollEvent scrollEvent((float)xoffset, (float)yoffset);

				data->eventCallbackFn(scrollEvent);
			});
	}
}

#endif