#include "pch"
#include "GLFW_monitor.h"
#include <core/app/Log.h>

namespace Xen {

	std::vector<Ref<Monitor>> GLFW_monitor::GetAvailableMonitors()
	{
		std::vector<Ref<Monitor>> to_return;

		for (int i = 0; i < Monitor::GetMonitorCount(); i++)
			to_return.push_back(Monitor::GetMonitor(i + 1));

		return to_return;
	}

	GLFW_monitor::GLFW_monitor(uint8_t index)
	{
		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count); 

		if (index < 1 || index > count)
		{
			XEN_ENGINE_LOG_ERROR("'index' in Monitor::GetMonitor() is out of bounds!");
			TRIGGER_BREAKPOINT;
		}

		m_Monitor = monitors[index - 1];
		m_CurrentMonitorVidMode = glfwGetVideoMode(m_Monitor);
	}

	uint8_t GLFW_monitor::GetMonitorCount()
	{
		int count;
		glfwGetMonitors(&count);
		return (uint8_t)count;
	}

	uint32_t GLFW_monitor::GetMonitorWidth() const						{ return (uint32_t)m_CurrentMonitorVidMode->width; }
	uint32_t GLFW_monitor::GetMonitorHeight() const						{ return (uint32_t)m_CurrentMonitorVidMode->height; }
	uint32_t GLFW_monitor::GetMonitorRefreshRate() const				{ return (uint32_t)m_CurrentMonitorVidMode->refreshRate; }
	void* GLFW_monitor::GetNativeMonitor() const						{ return m_Monitor; }
}