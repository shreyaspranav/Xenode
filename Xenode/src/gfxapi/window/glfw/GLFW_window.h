#pragma once

#include <core/app/Window.h>
#include <GLFW/glfw3.h>

#include <core/app/EventDispatcher.h>

namespace Xen {

	struct UserPointer
	{
		WindowProps props;
		EventDispatcher dispatcher;

		uint8_t monitor_count;
		GLFWmonitor** monitors;
	};

	class XEN_API GLFW_window : public Window
	{
	private:
		GLFWwindow* m_Window;
		UserPointer m_UserPointer;

	public:
		GLFW_window(const WindowProps& props);
		GLFW_window() {}
		virtual ~GLFW_window() { GLFW_window::Shutdown(); }

		void Create() override;
		void Update() override;
		void Shutdown() override;

		void SetVsync(bool enabled) override;

		virtual void* GetNativeWindow() const override { return m_Window; }

		inline uint32_t GetWidth() const override 
		{
			int width;
			glfwGetWindowSize(m_Window, &width, nullptr);
			return (uint32_t)width;
		}

		inline uint32_t GetHeight() const override
		{
			int height;
			glfwGetWindowSize(m_Window, nullptr, &height);
			return (uint32_t)height;
		}

		void SetFullScreenMonitor(const Ref<Monitor>& monitor) override;
		void SetWindowed() override;

		inline const std::string& GetTitle() const override { return m_UserPointer.props.title; }
		inline bool IsVsync() const override { return m_UserPointer.props.vsync; }

		void SetupEventListeners(const EventDispatcher& dispacher) override;
	};

}

