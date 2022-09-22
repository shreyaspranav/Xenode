#pragma once
#include <Core.h>

#include "EventDispatcher.h"

namespace Xen {
	class XEN_API GameApplication
	{
	private:
		EventDispatcher dispatcher;
		bool is_Running;

	public:
		GameApplication();
		virtual ~GameApplication();

		void Run();

		// Events:
		void OnWindowMoveEvent(Event& event);
		void OnWindowResizeEvent(Event& event);
		void OnWindowCloseEvent(Event& event);
		void OnWindowFocusEvent(Event& event);
		void OnWindowMinimizeEvent(Event& event);
		void OnWindowMaximizeEvent(Event& event);

		void OnKeyPressEvent(Event& event);
		void OnKeyReleaseEvent(Event& event);

		void OnMouseEnterEvent(Event& event);
		void OnMouseMoveEvent(Event& event);
		void OnMouseButtonPressEvent(Event& event);
		void OnMouseButtonReleaseEvent(Event& event);
		void OnMouseScrollEvent(Event& event);
	};
	XEN_API GameApplication* CreateApplication();
}

