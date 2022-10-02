#include "pch"
#include "GLFW_input.h"
#include <gfxapi/window/glfw/GLFW_window.h>

#include <core/app/Log.h>

namespace Xen {

	void GLFW_input::SetupInputListeners() const
	{
		// Key Callback:------

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				UserPointer pointer = *(UserPointer*)glfwGetWindowUserPointer(window);

				if (action == GLFW_PRESS)
				{
					KeyPressEvent e((KeyCode)key, 0);
					pointer.dispatcher.PostEvent(e);
				}

				else if (action == GLFW_REPEAT)
				{
					KeyPressEvent e((KeyCode)key, 1);
					pointer.dispatcher.PostEvent(e);
				}

				else if (action == GLFW_RELEASE)
				{
					KeyReleaseEvent e((KeyCode)key);
					pointer.dispatcher.PostEvent(e);
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int char_code)
			{
				UserPointer pointer = *(UserPointer*)glfwGetWindowUserPointer(window);
				CharEnterEvent e(char_code);
				pointer.dispatcher.PostEvent(e);
			});

		// Mouse Callbacks:------
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				UserPointer pointer = *(UserPointer*)glfwGetWindowUserPointer(window);

				if (action == GLFW_PRESS)
				{
					MouseButtonPressEvent e((MouseKeyCode)button);
					pointer.dispatcher.PostEvent(e);
				}

				if (action == GLFW_RELEASE)
				{
					MouseButtonReleaseEvent e((MouseKeyCode)button);
					pointer.dispatcher.PostEvent(e);
				}
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) 
			{
				UserPointer pointer = *(UserPointer*)glfwGetWindowUserPointer(window);
				MouseMoveEvent e((uint16_t)xpos, (uint16_t)ypos);
				pointer.dispatcher.PostEvent(e);
			});

		glfwSetCursorEnterCallback(m_Window, [](GLFWwindow* window, int entered) 
			{
				UserPointer pointer = *(UserPointer*)glfwGetWindowUserPointer(window);
				MouseEnterEvent e(entered);
				pointer.dispatcher.PostEvent(e);
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				UserPointer pointer = *(UserPointer*)glfwGetWindowUserPointer(window);
				MouseScrollEvent e(xoffset, yoffset);
				pointer.dispatcher.PostEvent(e);
			});
	}

	uint16_t GLFW_input::GetMouseX() const
	{
		double to_return;
		glfwGetCursorPos(m_Window, &to_return, nullptr);
		return (uint16_t)to_return;
	}
	uint16_t GLFW_input::GetMouseY() const
	{
		double to_return;
		glfwGetCursorPos(m_Window, nullptr, &to_return);
		return (uint16_t)to_return;
	}
}