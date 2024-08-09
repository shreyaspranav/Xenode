#pragma once
#include <Core.h>

#ifdef XEN_DEVICE_DESKTOP

#include <core/app/desktop/Window.h>
#include <core/app/EventDispatcher.h>

struct GLFWwindow;
struct GLFWcursor;
struct GLFWmonitor;

namespace Xen {


	struct WindowData
	{
		WindowProps props;
		
		EventCallbackFn eventCallbackFn;

		uint8_t monitorCount;
		GLFWmonitor** monitors;
	};

	class XEN_API GLFW_window : public Window
	{
	private:
		GLFWwindow* m_Window;
		GLFWcursor* m_Cursor;

		WindowData m_WindowData;
		 
	public:
		GLFW_window(const WindowProps& props);
		GLFW_window() {}
		virtual ~GLFW_window() { GLFW_window::Shutdown(); }

		void Create() override;
		void Update() override;
		void Shutdown() override;

		void SetRenderingAPI(GraphicsAPI api) override { m_WindowData.props.api = api; }

		void SetWindowResolution(uint32_t width, uint32_t height) override;
		void SetWindowMaxResolution(uint32_t width, uint32_t height) override;
		void SetWindowMinResolution(uint32_t width, uint32_t height) override;
		void SetWindowTitle(const std::string& title) override;

		void SetWindowIcon(const std::string& icon_path) override;
		void SetCursorIcon(const std::string& icon_path, uint16_t pointer_x, uint16_t pointer_y) override;

		void MinimizeWindow() override;
		void MaximizeWindow() override;

		void ShowWindow(bool show) override;
		void FocusWindow() override;

		void SetVsync(bool enabled) override;

		virtual void* GetNativeWindow() const override { return m_Window; }

		inline uint32_t GetWidth() const override;
		inline uint32_t GetHeight() const override;
		inline uint32_t GetFrameBufferWidth() const override;
		inline uint32_t GetFrameBufferHeight() const override;

		void SetFullScreenMonitor(const Ref<Monitor>& monitor) override;
		void SetWindowed() override;

		inline const std::string& GetTitle() const override { return m_WindowData.props.title; }
		inline bool IsVsync() const override { return m_WindowData.props.vsync; }

		void SetEventCallbackFunction(const EventCallbackFn& callbackFn) override { m_WindowData.eventCallbackFn = callbackFn; }
	};
}

#endif
