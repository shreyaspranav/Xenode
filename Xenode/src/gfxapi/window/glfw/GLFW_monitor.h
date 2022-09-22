#pragma once

#include <Core.h>
#include <core/app/Monitor.h>

#include <GLFW/glfw3.h>

namespace Xen {
	class XEN_API GLFW_monitor : public Monitor
	{
	private:
		GLFWmonitor* m_Monitor;
		const mutable GLFWvidmode* m_CurrentMonitorVidMode;

	public:
		GLFW_monitor(uint8_t index);
		virtual ~GLFW_monitor() {}

		uint32_t GetMonitorWidth() const override;
		uint32_t GetMonitorHeight() const override;
		uint32_t GetMonitorRefreshRate() const override;

		void* GetNativeMonitor() const override;
	};
}

