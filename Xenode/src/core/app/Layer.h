#pragma once

#include <Core.h>
#include "Events.h"

namespace Xen {
	class XEN_API Layer
	{
	public:
		uint32_t layerID;
		std::string layerName;

	public:
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(double timestep) {}
		virtual void OnFixedUpdate() {}
		virtual void OnRender() {}

		virtual void OnImGuiUpdate() {}

		virtual void OnWindowMoveEvent(WindowMoveEvent& event) {}
		virtual void OnWindowResizeEvent(WindowResizeEvent& event) {}
		virtual void OnWindowCloseEvent(WindowCloseEvent& event) {}
		virtual void OnWindowFocusEvent(WindowFocusEvent& event) {}
		virtual void OnWindowMinimizeEvent(WindowMinimizeEvent& event) {}
		virtual void OnWindowMaximizeEvent(WindowMaximizeEvent& event) {}

		virtual void OnKeyPressEvent(KeyPressEvent& event) {}
		virtual void OnKeyReleaseEvent(KeyReleaseEvent& event) {}
		virtual void OnCharEnterEvent(CharEnterEvent& event) {}

		virtual void OnMouseEnterEvent(MouseEnterEvent& event) {}
		virtual void OnMouseMoveEvent(MouseMoveEvent& event) {}
		virtual void OnMouseButtonPressEvent(MouseButtonPressEvent& event) {}
		virtual void OnMouseButtonReleaseEvent(MouseButtonReleaseEvent& event) {}
		virtual void OnMouseScrollEvent(MouseScrollEvent& event) {}
	};
}