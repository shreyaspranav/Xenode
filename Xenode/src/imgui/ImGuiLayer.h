#pragma once

#include <core/app/Layer.h>
#include <Core.h>

#include <GLFW/glfw3.h>

namespace Xen {
	class XEN_API ImGuiLayer : public Layer
	{
	private:
		GLFWwindow* m_Window;
	public:
		ImGuiLayer(GLFWwindow* window) : m_Window(window) {}
		virtual ~ImGuiLayer() {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(double timestep) override;

		virtual void OnWindowMoveEvent(WindowMoveEvent& event) override;
		virtual void OnWindowResizeEvent(WindowResizeEvent& event) override;
		virtual void OnWindowCloseEvent(WindowCloseEvent& event) override;
		virtual void OnWindowFocusEvent(WindowFocusEvent& event) override;
		virtual void OnWindowMinimizeEvent(WindowMinimizeEvent& event) override;
		virtual void OnWindowMaximizeEvent(WindowMaximizeEvent& event) override;

		virtual void OnKeyPressEvent(KeyPressEvent& event) override;
		virtual void OnKeyReleaseEvent(KeyReleaseEvent& event) override;

		virtual void OnMouseEnterEvent(MouseEnterEvent& event) override;
		virtual void OnMouseMoveEvent(MouseMoveEvent& event) override;
		virtual void OnMouseButtonPressEvent(MouseButtonPressEvent& event) override;
		virtual void OnMouseButtonReleaseEvent(MouseButtonReleaseEvent& event) override;
		virtual void OnMouseScrollEvent(MouseScrollEvent& event) override;
	};
}

