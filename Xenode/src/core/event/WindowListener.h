#pragma once

#include "ActionListener.h"

namespace Xen {

	struct WindowListenerData
	{
		uint16_t windowXPos, windowYpos;

		uint16_t window_width, window_height;
		uint16_t framebuffer_width, framebuffer_height;

		bool minimised, focused;
	};

	class XEN_API WindowListener : public ActionListener
	{
	public:
		virtual void OnWindowClose(const WindowListenerData& data) = 0;
		virtual void OnWindowMinimise(const WindowListenerData& data) = 0;

		virtual void OnWindowFocus(const WindowListenerData& data) = 0;
		virtual void OnWindowLostFocus(const WindowListenerData& data) = 0;

		virtual void OnWindowResize(const WindowListenerData& data) = 0;
		virtual void OnWindowMoved(const WindowListenerData& data) = 0;
	};
}