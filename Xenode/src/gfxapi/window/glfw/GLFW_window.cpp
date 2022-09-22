#include "pch"
#include "GLFW_window.h"

#include <core/app/Log.h>
#include <core/app/GameApplication.h>

namespace Xen {
	 
	Scope<Window> Window::GetWindow(const WindowProps& props) { return std::make_unique<GLFW_window>(props); }
	GLFW_window::GLFW_window(const WindowProps& props)
	{
		m_UserPointer.props = props;
		if (!glfwInit())
		{
			XEN_ENGINE_LOG_ERROR_SEVERE("GLFW failed to initialise!");
			TRIGGER_BREAKPOINT;
		}
	}

	void GLFW_window::Create()
	{
		m_Window = glfwCreateWindow(m_UserPointer.props.width, m_UserPointer.props.height, m_UserPointer.props.title.c_str(), NULL, NULL);

		if (!m_Window)
		{
			GLFW_window::Shutdown();
			XEN_ENGINE_LOG_ERROR_SEVERE("Failed to create a GLFW window!");
			TRIGGER_BREAKPOINT;
		}

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowPos(m_Window, (mode->width - m_UserPointer.props.width) / 2, (mode->height - m_UserPointer.props.height) / 2);

		glfwSetWindowUserPointer(m_Window, &m_UserPointer);

		int monitor_c;
		m_UserPointer.monitors = glfwGetMonitors(&monitor_c);
		m_UserPointer.monitor_count = (uint8_t)monitor_c;

		glfwSetMonitorUserPointer(glfwGetPrimaryMonitor(), &m_UserPointer);

		glfwSwapInterval(m_UserPointer.props.vsync);

		int count;
		const GLFWvidmode* modes = glfwGetVideoMode(glfwGetPrimaryMonitor());

		//for (int i = 0; i < count; i++) {
		//	XEN_ENGINE_LOG_WARN("{0}: Width:{1}", i, modes[i].width);
		//	XEN_ENGINE_LOG_WARN("{0}: Height:{1}", i, modes[i].height);
		//	XEN_ENGINE_LOG_WARN("{0}: RefreshRate:{1}", i, modes[i].refreshRate);
		//
		//}

		XEN_ENGINE_LOG_ERROR_SEVERE("Width: {0}", modes->width);
		XEN_ENGINE_LOG_ERROR_SEVERE("Height: {0}", modes->height);
		XEN_ENGINE_LOG_ERROR_SEVERE("Refresh Rate: {0}", modes->refreshRate);
	}

	void GLFW_window::SetFullScreenMonitor(const Ref<Monitor>& monitor)
	{
		glfwSetWindowMonitor(m_Window, (GLFWmonitor*)monitor->GetNativeMonitor(), 0, 0, m_UserPointer.props.width, m_UserPointer.props.height, GLFW_DONT_CARE);
	}

	void GLFW_window::SetWindowed()
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowPos(m_Window, (mode->width - m_UserPointer.props.width) / 2, (mode->height - m_UserPointer.props.height) / 2);

		glfwSetWindowMonitor(m_Window, NULL, (mode->width - m_UserPointer.props.width) / 2, (mode->height - m_UserPointer.props.height) / 2, m_UserPointer.props.width, m_UserPointer.props.height, GLFW_DONT_CARE);
	}

	void GLFW_window::SetupEventListeners(const EventDispatcher& dispatcher)
	{
		m_UserPointer.dispatcher = dispatcher;

		// Window Callbacks:-----
		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos)
			{
				WindowMoveEvent e((uint32_t)xpos, (uint32_t)ypos);
				UserPointer pointer = *(UserPointer*)glfwGetWindowUserPointer(window);
				pointer.dispatcher.PostEvent(e);
			});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowResizeEvent e((uint32_t)width, (uint32_t)height);
				UserPointer pointer = *(UserPointer*)glfwGetWindowUserPointer(window);
				pointer.dispatcher.PostEvent(e);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowCloseEvent e(WindowCloseEvent::USER_EXIT);
				UserPointer pointer = *(UserPointer*)glfwGetWindowUserPointer(window);
				pointer.dispatcher.PostEvent(e);
			});

		//glfwSetWindowRefreshCallback(m_Window, [](GLFWwindow* window)
		//	{
		//
		//	});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused)
			{
				WindowFocusEvent e((bool)focused);
				UserPointer pointer = *(UserPointer*)glfwGetWindowUserPointer(window);
				pointer.dispatcher.PostEvent(e);
			});

		glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* window, int iconified)
			{
				WindowMinimizeEvent e((bool)iconified);
				UserPointer pointer = *(UserPointer*)glfwGetWindowUserPointer(window);
				pointer.dispatcher.PostEvent(e);
			});

		glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximized)
			{
				WindowMaximizeEvent e((bool)maximized);
				UserPointer pointer = *(UserPointer*)glfwGetWindowUserPointer(window);
				pointer.dispatcher.PostEvent(e);
			});

		//glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		//	{
		//
		//	});

		// Monitor Events:-----------------

		glfwSetMonitorCallback([](GLFWmonitor* monitor, int event)
			{
				if (event == GLFW_CONNECTED)
				{
					UserPointer pointer = *(UserPointer*)glfwGetMonitorUserPointer(glfwGetPrimaryMonitor());
					pointer.monitor_count++;
				}
				else if (event == GLFW_DISCONNECTED)
				{
					UserPointer pointer = *(UserPointer*)glfwGetMonitorUserPointer(glfwGetPrimaryMonitor());
					pointer.monitor_count--;
				}
			});
		
	}

	void GLFW_window::Update()
	{
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}
	void GLFW_window::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}
	void GLFW_window::SetVsync(bool enabled)
	{

	}
}