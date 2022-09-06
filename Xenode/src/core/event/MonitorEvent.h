#pragma once

#include "ActionListener.h"

namespace Xen {
	class XEN_API MonitorListener : public ActionListener
	{
		virtual void OnMonitorConnect(uint8_t monitor_count) = 0;
		virtual void OnMonitorDisconnect(uint8_t monitor_count) = 0;
	};
}