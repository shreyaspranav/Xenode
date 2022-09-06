#pragma once

#include "ActionListener.h"
#include "core/app/KeyCodes.h"

namespace Xen {
	enum MouseScrollDir { SCROLL_UP, SCROLL_DOWN, SCROLL_STATIONARY };

	struct MouseListenerData 
	{
		uint16_t mouseX, mouseY;
		KeyCode button_pressed;

		MouseScrollDir scroll_dir;
	};

	class XEN_API MouseListener : public ActionListener
	{
		virtual void OnMouseMoved(const MouseListenerData& data) = 0;

		virtual void OnMouseButtonPressed(const MouseListenerData& data) = 0;
		virtual void OnMouseButtonReleased(const MouseListenerData& data) = 0;

		virtual void OnMouseEnteredSurface(const MouseListenerData& data) = 0;
		virtual void OnMouseExitedSurface(const MouseListenerData& data) = 0;

		virtual void OnMouseScrolled(const MouseListenerData& data) = 0;
	};
}