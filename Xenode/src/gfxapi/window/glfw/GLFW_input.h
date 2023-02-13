#pragma once

#include <core/app/Input.h>

struct GLFWwindow;

namespace Xen {
	class XEN_API GLFW_input : public Input
	{
	private:
		GLFWwindow* m_Window;

	public:
		void SetNativeWindow(void* window) override { m_Window = (GLFWwindow*)window; }
		void SetWindow(const Ref<Window>& window) override { m_Window = (GLFWwindow*)window->GetNativeWindow(); }

		void SetupInputListeners() const override;

		bool IsKeyPressed(KeyCode code) const override;
		bool IsMouseButtonPressed(MouseKeyCode code) const override;

		uint16_t GetMouseX() const override;
		uint16_t GetMouseY() const override;
	};
}

