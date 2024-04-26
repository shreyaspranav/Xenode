#include "pch"
#include "Monitor.h"
#include "DesktopApplication.h"

#include "gfxapi/window/glfw/GLFW_monitor.h"

namespace Xen {
	std::vector<Ref<Monitor>> Monitor::GetAvailableMonitors() 
	{ 
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return GLFW_monitor::GetAvailableMonitors();
		}
		return {};
	}
	Ref<Monitor> Monitor::GetMonitor(uint8_t index) {
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<GLFW_monitor>(index);
		}
		return {};
	}
	uint8_t Monitor::GetMonitorCount()
	{
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return GLFW_monitor::GetMonitorCount();
		}
		return 0;
	}
}