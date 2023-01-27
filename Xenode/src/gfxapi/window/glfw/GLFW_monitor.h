#pragma once

#include <Core.h>
#include <core/app/Monitor.h>

struct GLFWmonitor;
struct GLFWvidmode;

namespace Xen {
	class XEN_API GLFW_monitor : public Monitor
	{
	private:
		GLFWmonitor* m_Monitor;
		const mutable GLFWvidmode* m_CurrentMonitorVidMode;

	public:
		GLFW_monitor(uint8_t index);
		virtual ~GLFW_monitor() {}

		static std::vector<Ref<Monitor>> GetAvailableMonitors();
		static Ref<Monitor> GetMonitor(uint8_t index);
		static uint8_t GetMonitorCount();

		uint32_t GetMonitorWidth() const override;
		uint32_t GetMonitorHeight() const override;
		uint32_t GetMonitorRefreshRate() const override;

		void* GetNativeMonitor() const override;
	};
}

