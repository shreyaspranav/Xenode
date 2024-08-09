#include "pch"
#include "Monitor.h"

#ifdef XEN_DEVICE_DESKTOP

#include <gfxapi/window/glfw/GLFW_monitor.h>

namespace Xen {
	std::vector<Ref<Monitor>> Monitor::GetAvailableMonitors() 
	{

#ifdef XEN_DEVICE_DESKTOP
		return GLFW_monitor::GetAvailableMonitors();
#elif defined(XEN_DEVICE_MOBILE)
		TRIGGER_BREAKPOINT;
#endif
	}
	Ref<Monitor> Monitor::GetMonitor(uint8_t index) 
	{
#ifdef XEN_DEVICE_DESKTOP
		return std::make_shared<GLFW_monitor>(index);
#elif defined(XEN_DEVICE_MOBILE)
		TRIGGER_BREAKPOINT;
#endif
	}
	uint8_t Monitor::GetMonitorCount()
	{
#ifdef XEN_DEVICE_DESKTOP
		return GLFW_monitor::GetMonitorCount();
#elif defined(XEN_DEVICE_MOBILE)
		TRIGGER_BREAKPOINT;
#endif
	}
}

#endif