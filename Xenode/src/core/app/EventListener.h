#pragma once

#include <Core.h>
#include "KeyCodes.h"

namespace Xen {

	struct KeyListenerData
	{
		KeyCode code;
		bool isRepeat;
	};

	enum MouseScrollDir { SCROLL_UP, SCROLL_DOWN, SCROLL_STATIONARY };

	struct WindowListenerData
	{
		uint16_t windowXPos, windowYpos;

		uint16_t window_width, window_height;
		uint16_t framebuffer_width, framebuffer_height;

		bool minimised, focused;
	};

	class XEN_API ActionListener
	{
	public:
		virtual void ActionPerformed() = 0;
	};

	class XEN_API KeyListener : public ActionListener
	{
		virtual void OnKeyPressed(const KeyListenerData& data) = 0;
		virtual void OnKeyReleased(const KeyListenerData& data) = 0;
	};

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