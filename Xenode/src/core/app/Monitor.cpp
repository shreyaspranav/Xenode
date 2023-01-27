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

		case GraphicsAPI::XEN_VULKAN_API:
			return {};

		case GraphicsAPI::XEN_DIRECT3D_API:
			return {};

		case GraphicsAPI::XEN_OPENGLES_API:
			return {};

		case GraphicsAPI::XEN_METAL_API:
			return {};
		}
	}
	Ref<Monitor> Monitor::GetMonitor(uint8_t index) {
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<GLFW_monitor>(index);

		case GraphicsAPI::XEN_VULKAN_API:
			return nullptr;

		case GraphicsAPI::XEN_DIRECT3D_API:
			return nullptr;

		case GraphicsAPI::XEN_OPENGLES_API:
			return nullptr;

		case GraphicsAPI::XEN_METAL_API:
			return nullptr;

		}
	}
	uint8_t Monitor::GetMonitorCount()
	{
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return GLFW_monitor::GetMonitorCount();

		case GraphicsAPI::XEN_VULKAN_API:
			return 0;

		case GraphicsAPI::XEN_DIRECT3D_API:
			return 0;

		case GraphicsAPI::XEN_OPENGLES_API:
			return 0;

		case GraphicsAPI::XEN_METAL_API:
			return 0;
		}
	}
}