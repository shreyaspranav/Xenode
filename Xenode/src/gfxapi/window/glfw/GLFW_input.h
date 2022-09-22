#pragma once

#include <core/app/Input.h>
#include <GLFW/glfw3.h>

namespace Xen {
	class XEN_API GLFW_input : public Input
	{
	private:
		GLFWwindow* m_Window;

	public:
		void SetNativeWindow(void* window) override { m_Window = (GLFWwindow*)window; }
		void SetWindow(const Scope<Window>& window) override { m_Window = (GLFWwindow*)window->GetNativeWindow(); }

		void SetupInputListeners() const override;

		bool IsKeyPressed(KeyCode code) const override { return glfwGetKey(m_Window, (int)code); }
		bool IsMouseButtonPressed(MouseKeyCode code) const override { return glfwGetMouseButton(m_Window, (int)code); }

		uint16_t GetMouseX() const override;
		uint16_t GetMouseY() const override;
	};
}

