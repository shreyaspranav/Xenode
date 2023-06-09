#pragma once

#include <core/app/Layer.h>
#include <Core.h>

#ifdef XEN_DEVICE_DESKTOP
#include <core/app/Window.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif

#include "IconsFontAwesome.h"

namespace Xen {
	class XEN_API ImGuiLayer : public Layer
	{
	private:
		Ref<Window> m_Window;
		bool m_FirstTime = true;
	public:
		ImGuiLayer() {}
		virtual ~ImGuiLayer() {}

		void SetWindow(const Ref<Window>& window) { m_Window = window; }

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(double timestep) override;

		virtual void OnImGuiUpdate() override;

		void Begin();
		void End();

		virtual void OnWindowMoveEvent(WindowMoveEvent& event) override;
		virtual void OnWindowResizeEvent(WindowResizeEvent& event) override;
		virtual void OnWindowCloseEvent(WindowCloseEvent& event) override;
		virtual void OnWindowFocusEvent(WindowFocusEvent& event) override;
		virtual void OnWindowMinimizeEvent(WindowMinimizeEvent& event) override;
		virtual void OnWindowMaximizeEvent(WindowMaximizeEvent& event) override;

		virtual void OnKeyPressEvent(KeyPressEvent& event) override;
		virtual void OnKeyReleaseEvent(KeyReleaseEvent& event) override;
		virtual void OnCharEnterEvent(CharEnterEvent& event) override;

		virtual void OnMouseEnterEvent(MouseEnterEvent& event) override;
		virtual void OnMouseMoveEvent(MouseMoveEvent& event) override;
		virtual void OnMouseButtonPressEvent(MouseButtonPressEvent& event) override;
		virtual void OnMouseButtonReleaseEvent(MouseButtonReleaseEvent& event) override;
		virtual void OnMouseScrollEvent(MouseScrollEvent& event) override;
	};
}

