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
		ImFontConfig config;
		config.MergeMode = true;
		//config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced

		static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

		io.Fonts->AddFontFromFileTTF("assets/fonts/fa-solid-900.ttf", 14, &config, icon_ranges);
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		ImGuiStyle* style = &ImGui::GetStyle();
		style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_Window->GetNativeWindow(), false);
		ImGui_ImplOpenGL3_Init("#version 130 core");
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

		// Color theme: 
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.31f, 0.31f, 0.31f, 0.51f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.88f, 0.24f, 0.24f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.88f, 0.24f, 0.24f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.34f, 0.34f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.27f, 0.27f, 0.27f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.88f, 0.24f, 0.24f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.34f, 0.34f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.67f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.75f, 0.10f, 0.10f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.75f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.86f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 0.00f, 0.00f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 0.30f, 0.30f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.18f, 0.86f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.22f, 0.22f, 0.22f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.98f, 0.26f, 0.26f, 0.35f);
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