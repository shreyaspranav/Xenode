#pragma once

#include <Core.h>

#ifdef XEN_DEVICE_DESKTOP

namespace Xen {
	class XEN_API Monitor
	{
	public:
		virtual uint32_t GetMonitorWidth() const = 0;
		virtual uint32_t GetMonitorHeight() const = 0;
		virtual uint32_t GetMonitorRefreshRate() const = 0;

		virtual uint8_t GetMonitorRedBits() const = 0;
		virtual uint8_t GetMonitorGreenBits() const = 0;
		virtual uint8_t GetMonitorBlueBits() const = 0;

		virtual void* GetNativeMonitor() const = 0;

		static std::vector<Ref<Monitor>> GetAvailableMonitors();
		static Ref<Monitor> GetMonitor(uint8_t index);
		static uint8_t GetMonitorCount();
	};
}

#endif