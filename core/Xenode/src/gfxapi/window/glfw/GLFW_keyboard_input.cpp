#include "pch"
#include "GLFW_keyboard_input.h"

#ifdef XEN_DEVICE_DESKTOP

#include "GLFW_window.h"

#include <core/app/Events.h>

#include <GLFW/glfw3.h>

namespace Xen
{
	struct GLFW_keyboard_input_state 
	{
		GLFWwindow* currentWindow;
	}keyboardInputState;

	// Private functions: -------------------------------------------------------------------------------------------------------
	static KeyAction ToKeyAction(int keyAction)
	{
		switch (keyAction)
		{
		case GLFW_PRESS:	return KeyAction::Press;
		case GLFW_RELEASE:	return KeyAction::Release;
		case GLFW_REPEAT:	return KeyAction::Hold;
		default:
			TRIGGER_BREAKPOINT;
			return KeyAction::Press;
			break;
		}
	}

	// Implementation: ----------------------------------------------------------------------------------------------------------
	bool GLFW_keyboard_input::IsKeyPressed(KeyboardKeyCode keyCode)
	{
		// Change this if you plan to change the keycodes.
		int glfwKey = (int)keyCode;

		return glfwGetKey(keyboardInputState.currentWindow, glfwKey) == GLFW_PRESS ? true : false;
	}

	void GLFW_keyboard_input::Init(const Ref<Window>& window)
	{
		// Set the window in the current state.
		// TODO: Make sure that 'window' is a GLFW window
		keyboardInputState.currentWindow = (GLFWwindow*)window->GetNativeWindow();

		// Setup all the callbacks and raise a event if a callback has occured.

		// Key Callback, callback whenever a key has pressed, released or held.
		glfwSetKeyCallback(keyboardInputState.currentWindow, 
			[](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);

				KeyboardEvent keyboardEvent((KeyboardKeyCode)key, ToKeyAction(action));
				data->eventCallbackFn(keyboardEvent);
			});

		// Similar to key callback, but instead of telling the key, it tells the unicode instead and calls only for key presses.
		glfwSetCharCallback(keyboardInputState.currentWindow,
			[](GLFWwindow* window, unsigned int codepoint) 
			{
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);

				CharEnterEvent charEnterEvent(codepoint);
				data->eventCallbackFn(charEnterEvent);
			});
	}
}

#endif