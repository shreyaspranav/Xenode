#include "pch"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace Xen {
	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		io.Fonts->AddFontFromFileTTF("assets/fonts/DroidSans.ttf", 15);

		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_Window->GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330 core");
	}
	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiUpdate()
	{
	}

	void ImGuiLayer::OnUpdate(double timestep)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	void ImGuiLayer::OnWindowMoveEvent(WindowMoveEvent& event)
	{
	}
	void ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent& event)
	{
	}
	void ImGuiLayer::OnWindowCloseEvent(WindowCloseEvent& event)
	{
	}
	void ImGuiLayer::OnWindowFocusEvent(WindowFocusEvent& event)
	{
	}
	void ImGuiLayer::OnWindowMinimizeEvent(WindowMinimizeEvent& event)
	{
	}
	void ImGuiLayer::OnWindowMaximizeEvent(WindowMaximizeEvent& event)
	{
	}
	void ImGuiLayer::OnKeyPressEvent(KeyPressEvent& event)
	{
	}
	void ImGuiLayer::OnKeyReleaseEvent(KeyReleaseEvent& event)
	{
	}
	void ImGuiLayer::OnMouseEnterEvent(MouseEnterEvent& event)
	{
	}
	void ImGuiLayer::OnMouseMoveEvent(MouseMoveEvent& event)
	{
	}
	void ImGuiLayer::OnMouseButtonPressEvent(MouseButtonPressEvent& event)
	{
	}
	void ImGuiLayer::OnMouseButtonReleaseEvent(MouseButtonReleaseEvent& event)
	{
	}
	void ImGuiLayer::OnMouseScrollEvent(MouseScrollEvent& event)
	{
	}
}