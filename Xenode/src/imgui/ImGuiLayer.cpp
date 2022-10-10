#include "pch"
#include "ImGuiLayer.h"

#include <glad/gl.h>
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "core/app/Log.h"


namespace Xen {
	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		io.Fonts->AddFontFromFileTTF("assets/fonts/DroidSans.ttf", 15);
		io.Fonts->AddFontFromFileTTF("assets/fonts/sui.ttf", 16);
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		//io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;


		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_Window->GetNativeWindow(), false);
		ImGui_ImplOpenGL3_Init("#version 460 core");
	}
	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiUpdate()
	{
		//ImGui::ShowDemoWindow();
	}

	void ImGuiLayer::OnUpdate(double timestep)
	{

	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(m_Window->GetWidth(), m_Window->GetHeight());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::OnWindowMoveEvent(WindowMoveEvent& event)
	{
	}
	void ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(event.GetWidth(), event.GetHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	}
	void ImGuiLayer::OnWindowCloseEvent(WindowCloseEvent& event)
	{
	}
	void ImGuiLayer::OnWindowFocusEvent(WindowFocusEvent& event)
	{
		//ImGui_ImplGlfw_WindowFocusCallback((GLFWwindow*)m_Window->GetNativeWindow(), event.IsFocused());
	}
	void ImGuiLayer::OnWindowMinimizeEvent(WindowMinimizeEvent& event)
	{

	}
	void ImGuiLayer::OnWindowMaximizeEvent(WindowMaximizeEvent& event)
	{
	}
	void ImGuiLayer::OnKeyPressEvent(KeyPressEvent& event)
	{
		ImGui_ImplGlfw_KeyCallback((GLFWwindow*)m_Window->GetNativeWindow(), event.GetKey(), 0, GLFW_PRESS, 0);
	}
	void ImGuiLayer::OnKeyReleaseEvent(KeyReleaseEvent& event)
	{
		ImGui_ImplGlfw_KeyCallback((GLFWwindow*)m_Window->GetNativeWindow(), event.GetKey(), 0, GLFW_RELEASE, 0);
	}
	void ImGuiLayer::OnCharEnterEvent(CharEnterEvent& event)
	{
		ImGui_ImplGlfw_CharCallback((GLFWwindow*)m_Window->GetNativeWindow(), event.GetChar());
	}
	void ImGuiLayer::OnMouseEnterEvent(MouseEnterEvent& event)
	{
		ImGui_ImplGlfw_CursorEnterCallback((GLFWwindow*)m_Window->GetNativeWindow(), event.IsInWindow());
	}
	void ImGuiLayer::OnMouseMoveEvent(MouseMoveEvent& event)
	{
		//ImGui_ImplGlfw_CursorPosCallback((GLFWwindow*)m_Window->GetNativeWindow(), event.GetX(), event.GetY());
	}
	void ImGuiLayer::OnMouseButtonPressEvent(MouseButtonPressEvent& event)
	{
		ImGui_ImplGlfw_MouseButtonCallback((GLFWwindow*)m_Window->GetNativeWindow(), event.GetMouseKeyCode(), GLFW_PRESS, 0); // Implement Modifier keys in Your Event System!
	}
	void ImGuiLayer::OnMouseButtonReleaseEvent(MouseButtonReleaseEvent& event)
	{
		ImGui_ImplGlfw_MouseButtonCallback((GLFWwindow*)m_Window->GetNativeWindow(), event.GetMouseKeyCode(), GLFW_RELEASE, 0); // Implement Modifier keys in Your Event System!
	}
	void ImGuiLayer::OnMouseScrollEvent(MouseScrollEvent& event)
	{
		ImGui_ImplGlfw_ScrollCallback((GLFWwindow*)m_Window->GetNativeWindow(), event.GetXOffset(), event.GetYOffset());
	}
}