#include "pch"
#include "GLFW_window.h"

#ifdef XEN_DEVICE_DESKTOP

#include <core/app/Log.h>
#include <core/app/GameApplication.h>

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Xen {
	 
	GLFW_window::GLFW_window(const WindowProps& props)
	{
		m_WindowData.props = props;
		if (!glfwInit())
		{
			XEN_ENGINE_LOG_ERROR_SEVERE("GLFW failed to initialise!");
			TRIGGER_BREAKPOINT;
		}
	}

	void GLFW_window::Create()
	{
		if (m_WindowData.props.resizable)
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		else 
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		if (m_WindowData.props.api == GraphicsAPI::XEN_OPENGL_API)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
			#ifdef XEN_DEBUG
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
			#endif // XEN_DEBUG
		}

		else
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_Window = glfwCreateWindow(m_WindowData.props.width, m_WindowData.props.height, m_WindowData.props.title.c_str(), NULL, NULL);

		if (!m_Window)
		{
			GLFW_window::Shutdown();
			XEN_ENGINE_LOG_ERROR_SEVERE("Failed to create a GLFW window!");
			TRIGGER_BREAKPOINT;
		}

		glfwSetErrorCallback([](int error_code, const char* description) 
			{
				XEN_ENGINE_LOG_ERROR_SEVERE("GLFW Error: {0}", description);
				//TRIGGER_BREAKPOINT;
			});

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowPos(m_Window, (mode->width - m_WindowData.props.width) / 2, (mode->height - m_WindowData.props.height) / 2);

		glfwSetWindowUserPointer(m_Window, &m_WindowData);

		int monitorC;
		m_WindowData.monitors = glfwGetMonitors(&monitorC);
		m_WindowData.monitorCount = (uint8_t)monitorC;

		glfwSetMonitorUserPointer(glfwGetPrimaryMonitor(), &m_WindowData);
		
		glfwSwapInterval(m_WindowData.props.vsync);

		m_Cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		glfwSetCursor(m_Window, m_Cursor);

		// Window Callbacks:-----
		glfwSetWindowPosCallback(m_Window, 
			[](GLFWwindow* window, int xpos, int ypos)
			{
				WindowMoveEvent e((uint32_t)xpos, (uint32_t)ypos);
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
				data->eventCallbackFn(e);
			});

		glfwSetWindowSizeCallback(m_Window, 
			[](GLFWwindow* window, int width, int height)
			{
				WindowResizeEvent e((uint32_t)width, (uint32_t)height);
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
				data->eventCallbackFn(e);
			});

		glfwSetWindowCloseCallback(m_Window, 
			[](GLFWwindow* window)
			{
				WindowCloseEvent e(WindowCloseEvent::USER_EXIT);
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
				data->eventCallbackFn(e);
			});

		glfwSetWindowFocusCallback(m_Window, 
			[](GLFWwindow* window, int focused)
			{
				WindowFocusEvent e((bool)focused);
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
				data->eventCallbackFn(e);
			});

		glfwSetWindowIconifyCallback(m_Window, 
			[](GLFWwindow* window, int iconified)
			{
				WindowMinimizeEvent e((bool)iconified);
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
				data->eventCallbackFn(e);
			});

		glfwSetWindowMaximizeCallback(m_Window, 
			[](GLFWwindow* window, int maximized)
			{
				WindowMaximizeEvent e((bool)maximized);
				WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
				data->eventCallbackFn(e);
			});

		// Monitor Events:--------------------------------------------------------------------------------------------------------

		glfwSetMonitorCallback(
			[](GLFWmonitor* monitor, int event)
			{
				if (event == GLFW_CONNECTED)
				{
					WindowData* data = (WindowData*)glfwGetMonitorUserPointer(glfwGetPrimaryMonitor());
					data->monitorCount++;
				}
				else if (event == GLFW_DISCONNECTED)
				{
					WindowData* data = (WindowData*)glfwGetMonitorUserPointer(glfwGetPrimaryMonitor());
					data->monitorCount--;
				}
			});
	}


	void GLFW_window::SetFullScreenMonitor(const Ref<Monitor>& monitor)
	{
		glfwSetWindowMonitor(m_Window, (GLFWmonitor*)monitor->GetNativeMonitor(), 0, 0, m_WindowData.props.width, m_WindowData.props.height, GLFW_DONT_CARE);
	}

	void GLFW_window::SetWindowed()
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowPos(m_Window, (mode->width - m_WindowData.props.width) / 2, (mode->height - m_WindowData.props.height) / 2);

		glfwSetWindowMonitor(m_Window, NULL, 
			(mode->width - m_WindowData.props.width) / 2, 
			(mode->height - m_WindowData.props.height) / 2, 
			m_WindowData.props.width, 
			m_WindowData.props.height, 
			GLFW_DONT_CARE);
	}

	void GLFW_window::SetWindowIcon(const std::string& icon_path)
	{
		GLFWimage image[1];
		image[0].pixels = stbi_load(icon_path.c_str(), &image[0].width, &image[0].height, 0, 4);

		if (image[0].pixels == NULL)
		{
			XEN_ENGINE_LOG_ERROR("Failed to Load Window Icon!");
			TRIGGER_BREAKPOINT;
		}

		glfwSetWindowIcon(m_Window, 1, image);
	}

	void GLFW_window::SetCursorIcon(const std::string& icon_path, uint16_t pointer_x, uint16_t pointer_y)
	{
		GLFWimage image[1];
		image[0].pixels = stbi_load(icon_path.c_str(), &image[0].width, &image[0].height, 0, 4);

		if(image[0].pixels == NULL)
		{
			XEN_ENGINE_LOG_ERROR("Failed to Load Cursor Icon!");
			TRIGGER_BREAKPOINT;
		}

		m_Cursor = glfwCreateCursor(image, pointer_x, pointer_y);

		glfwSetCursor(m_Window, m_Cursor);
	}

	void GLFW_window::Update()
	{
		glfwPollEvents();
	}
	void GLFW_window::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	inline uint32_t GLFW_window::GetWidth() const
	{
		int width;
		glfwGetWindowSize(m_Window, &width, nullptr);
		return (uint32_t)width;
	}

	inline uint32_t GLFW_window::GetHeight() const
	{
		int height;
		glfwGetWindowSize(m_Window, nullptr, &height);
		return (uint32_t)height;
	}

	inline uint32_t GLFW_window::GetFrameBufferWidth() const
	{
		int width;
		glfwGetFramebufferSize(m_Window, &width, nullptr);
		return (uint32_t)width;
	}

	inline uint32_t GLFW_window::GetFrameBufferHeight() const
	{
		int height;
		glfwGetFramebufferSize(m_Window, nullptr, &height);
		return (uint32_t)height;
	}

	void GLFW_window::SetWindowResolution(uint32_t width, uint32_t height)			{ glfwSetWindowSize(m_Window, width, height); }
	void GLFW_window::SetWindowMaxResolution(uint32_t width, uint32_t height)		{ glfwSetWindowSizeLimits(m_Window, GLFW_DONT_CARE, GLFW_DONT_CARE, width, height); }
	void GLFW_window::SetWindowMinResolution(uint32_t width, uint32_t height)		{ glfwSetWindowSizeLimits(m_Window, width, height, GLFW_DONT_CARE, GLFW_DONT_CARE); }
	void GLFW_window::SetWindowTitle(const std::string& title)						{ glfwSetWindowTitle(m_Window, title.c_str()); }
	void GLFW_window::MinimizeWindow()												{ glfwIconifyWindow(m_Window); }
	void GLFW_window::MaximizeWindow()												{ glfwMaximizeWindow(m_Window); }

	void GLFW_window::ShowWindow(bool show)
	{
		if (show)
			glfwShowWindow(m_Window);
		else
			glfwHideWindow(m_Window);
	}
	void GLFW_window::FocusWindow()													{ glfwFocusWindow(m_Window); }
	void GLFW_window::SetVsync(bool enabled)										{ glfwSwapInterval(enabled); }
}

#endif