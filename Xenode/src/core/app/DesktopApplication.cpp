#include "pch"
#include "DesktopApplication.h"
#include "Log.h"
#include "LayerStack.h"
#include "Input.h"
#include "Monitor.h"
#include "Timer.h"

#include <imgui/ImGuiLayer.h>

namespace Xen {

	DesktopApplication::DesktopApplication()
	{
#ifdef XEN_DEVICE_DESKTOP
		is_Running = 1;
		dispatcher.SetEventCallbackFn(EventType::WindowMoveEvent, std::bind(&DesktopApplication::OnWindowMoveEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::WindowResizeEvent, std::bind(&DesktopApplication::OnWindowResizeEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::WindowCloseEvent, std::bind(&DesktopApplication::OnWindowCloseEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::WindowFocusEvent, std::bind(&DesktopApplication::OnWindowFocusEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::WindowMinimizeEvent, std::bind(&DesktopApplication::OnWindowMinimizeEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::WindowMaximizeEvent, std::bind(&DesktopApplication::OnWindowMaximizeEvent, this, std::placeholders::_1));

		dispatcher.SetEventCallbackFn(EventType::KeyPressEvent, std::bind(&DesktopApplication::OnKeyPressEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::KeyReleaseEvent, std::bind(&DesktopApplication::OnKeyReleaseEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::CharEnterEvent, std::bind(&DesktopApplication::OnCharEnterEvent, this, std::placeholders::_1));

		dispatcher.SetEventCallbackFn(EventType::MouseEnterEvent, std::bind(&DesktopApplication::OnMouseEnterEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::MouseMoveEvent, std::bind(&DesktopApplication::OnMouseMoveEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::MouseButtonPressEvent, std::bind(&DesktopApplication::OnMouseButtonPressEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::MouseButtonReleaseEvent, std::bind(&DesktopApplication::OnMouseButtonReleaseEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::MouseScrollEvent, std::bind(&DesktopApplication::OnMouseScrollEvent, this, std::placeholders::_1));


		window_width = 1600;
		window_height = 900;
		window_title = "Xenode Application";
		vsync = 1;
		resizable = 1;
		fullscreen_monitor = 0;
#endif

		stack = std::make_unique<LayerStack>(20);
		imgui_render = 0;

#ifdef XEN_DEVICE_DESKTOP
		m_Api = GraphicsAPI::XEN_OPENGL_API;
#elif XEN_DEVICE_MOBILE
		m_Api = GraphicsAPI::XEN_OPENGLES_API;
#endif
	}

	DesktopApplication::~DesktopApplication()
	{
		delete m_Context;
	}

	void DesktopApplication::PushLayer(const Ref<Layer>& layer) { stack->PushLayer(layer); }
	void DesktopApplication::PushLayer(const Ref<Layer>& layer, uint8_t loc) { stack->PushLayer(layer, loc); }
	void DesktopApplication::PopLayer() { stack->PopLayer(); }
	void DesktopApplication::PopLayer(uint8_t loc) { stack->PopLayer(loc); }

	void DesktopApplication::OnCreate()
	{
		OnCreate();
	}
	void DesktopApplication::OnStart()
	{
#ifdef XEN_DEVICE_DESKTOP
		WindowProps props(window_title, window_width, window_height, vsync, resizable, m_Api);
		window = Window::GetWindow(props);
		window->Create();
		window->SetupEventListeners(dispatcher);

		std::vector<Ref<Monitor>> monitors = Monitor::GetAvailableMonitors();
		uint8_t d = Monitor::GetMonitorCount();

		if (fullscreen_monitor != 0)
			window->SetFullScreenMonitor(monitors[fullscreen_monitor - 1]);

		window->SetWindowIcon("assets/icons/window_icon.png");
		//window->SetCursorIcon("assets/icons/cursor_icon.png", 14, 10);

		Scope<Input> input = Input::GetInputInterface();
		input->SetWindow(window);
		input->SetupInputListeners();

		m_Context = GraphicsContext::CreateContext(window);
		m_Context->Init();

		m_ImGuiLayer = std::make_shared<ImGuiLayer>();
		m_ImGuiLayer->SetWindow(window);
		PushLayer(m_ImGuiLayer);
#endif
		OnStart();
	}
	void DesktopApplication::OnUpdate(double timestep)
	{
		OnUpdate(timestep);
		for (int i = stack->GetCount(); i >= 1; i--)
			stack->GetLayer(i)->OnUpdate(timestep);
	}

	void* DesktopApplication::GetNativeWindow()
	{
		return window->GetNativeWindow();
	}

	void DesktopApplication::ImGuiRender()
	{
#ifdef XEN_DEVICE_DESKTOP
		if (imgui_render || imgui_always_render)
		{
			m_ImGuiLayer->Begin();
			for (int i = stack->GetCount(); i >= 1; i--)
				stack->GetLayer(i)->OnImGuiUpdate();
			m_ImGuiLayer->End();
		}
#endif // XEN_DEVICE_DESKTOP
	}

	void DesktopApplication::OnWindowMoveEvent(Event& event)
	{
		WindowMoveEvent& evt = static_cast<WindowMoveEvent&>(event);
		for (int i = stack->GetCount(); i >= 1; i--) {
			if (evt.handled)
				break;
			stack->GetLayer(i)->OnWindowMoveEvent(evt);
		}
	}

	void DesktopApplication::OnWindowResizeEvent(Event& event)
	{
		WindowResizeEvent& evt = static_cast<WindowResizeEvent&>(event);
		for (int i = stack->GetCount(); i >= 1; i--) {
			if (evt.handled)
				break;
			stack->GetLayer(i)->OnWindowResizeEvent(evt);
		}
	}

	void DesktopApplication::OnWindowCloseEvent(Event& event)
	{
		WindowCloseEvent& evt = static_cast<WindowCloseEvent&>(event);
		for (int i = stack->GetCount(); i >= 1; i--) {
			if (evt.handled)
				break;
			stack->GetLayer(i)->OnWindowCloseEvent(evt);
		}
		is_Running = 0;
	}

	void DesktopApplication::OnWindowFocusEvent(Event& event)
	{
		WindowFocusEvent& evt = static_cast<WindowFocusEvent&>(event);
		for (int i = stack->GetCount(); i >= 1; i--) {
			if (evt.handled)
				break;
			stack->GetLayer(i)->OnWindowFocusEvent(evt);
		}
	}

	void DesktopApplication::OnWindowMinimizeEvent(Event& event)
	{
		WindowMinimizeEvent& evt = static_cast<WindowMinimizeEvent&>(event);
		for (int i = stack->GetCount(); i >= 1; i--) {
			if (evt.handled)
				break;
			stack->GetLayer(i)->OnWindowMinimizeEvent(evt);
		}
	}

	void DesktopApplication::OnWindowMaximizeEvent(Event& event)
	{
		WindowMaximizeEvent& evt = static_cast<WindowMaximizeEvent&>(event);
		for (int i = stack->GetCount(); i >= 1; i--) {
			if (evt.handled)
				break;
			stack->GetLayer(i)->OnWindowMaximizeEvent(evt);
		}
	}

	void DesktopApplication::OnKeyPressEvent(Event& event)
	{
		KeyPressEvent& evt = static_cast<KeyPressEvent&>(event);

		if (evt.GetKey() == KeyCode::KEY_GRAVE_ACCENT && imgui_render == 0)
			imgui_render = 1;
		else if (evt.GetKey() == KeyCode::KEY_GRAVE_ACCENT && imgui_render == 1)
			imgui_render = 0;

		for (int i = stack->GetCount(); i >= 1; i--) {
			if (evt.handled)
				break;
			stack->GetLayer(i)->OnKeyPressEvent(evt);
		}
	}

	void DesktopApplication::OnKeyReleaseEvent(Event& event)
	{
		KeyReleaseEvent& evt = static_cast<KeyReleaseEvent&>(event);
		for (int i = stack->GetCount(); i >= 1; i--) {
			if (evt.handled)
				break;
			stack->GetLayer(i)->OnKeyReleaseEvent(evt);
		}
	}

	void DesktopApplication::OnCharEnterEvent(Event& event)
	{
		CharEnterEvent& evt = static_cast<CharEnterEvent&>(event);
		for (int i = stack->GetCount(); i >= 1; i--) {
			if (evt.handled)
				break;
			stack->GetLayer(i)->OnCharEnterEvent(evt);
		}
	}

	void DesktopApplication::OnMouseEnterEvent(Event& event)
	{
		MouseEnterEvent& evt = static_cast<MouseEnterEvent&>(event);
		for (int i = stack->GetCount(); i >= 1; i--) {
			if (evt.handled)
				break;
			stack->GetLayer(i)->OnMouseEnterEvent(evt);
		}
	}

	void DesktopApplication::OnMouseMoveEvent(Event& event)
	{
		MouseMoveEvent& evt = static_cast<MouseMoveEvent&>(event);
		for (int i = stack->GetCount(); i >= 1; i--) {
			if (evt.handled)
				break;
			stack->GetLayer(i)->OnMouseMoveEvent(evt);
		}
	}

	void DesktopApplication::OnMouseButtonPressEvent(Event& event)
	{
		MouseButtonPressEvent& evt = static_cast<MouseButtonPressEvent&>(event);
		for (int i = stack->GetCount(); i >= 1; i--) {
			if (evt.handled)
				break;
			stack->GetLayer(i)->OnMouseButtonPressEvent(evt);
		}
	}

	void DesktopApplication::OnMouseButtonReleaseEvent(Event& event)
	{
		MouseButtonReleaseEvent& evt = static_cast<MouseButtonReleaseEvent&>(event);
		for (int i = stack->GetCount(); i >= 1; i--) {
			if (evt.handled)
				break;
			stack->GetLayer(i)->OnMouseButtonReleaseEvent(evt);
		}
	}

	void DesktopApplication::OnMouseScrollEvent(Event& event)
	{
		MouseScrollEvent& evt = static_cast<MouseScrollEvent&>(event);
		for (int i = stack->GetCount(); i >= 1; i--) {
			if (evt.handled)
				break;
			stack->GetLayer(i)->OnMouseScrollEvent(evt);
		}
	}

	void DesktopApplication::OnRender()
	{
		OnRender();
		for (int i = stack->GetCount(); i >= 1; i--) { stack->GetLayer(i)->OnRender(); }
	}

	void DesktopApplication::OnFixedUpdate()
	{
		OnFixedUpdate();
		for (int i = stack->GetCount(); i >= 1; i--) { stack->GetLayer(i)->OnFixedUpdate(); }
	}

	void DesktopApplication::Run()
	{
		const double S_PER_UPDATE = 1.0 / 60.0;

		DesktopApplication::OnCreate();
		DesktopApplication::OnStart();

		double previous = Window::GetTime();
		double lag = 0.0;

		while (is_Running) {
			double current = Window::GetTime();
			double timestep = current - previous;
			previous = current;

			DesktopApplication::OnUpdate(timestep);
			lag += timestep;

			//XEN_ENGINE_LOG_INFO("FPS: {0}", (1.0 / timestep));

			while (lag >= S_PER_UPDATE)
			{
				DesktopApplication::OnFixedUpdate();
				lag -= S_PER_UPDATE;
			}
			// Run this function in a different thread:
			DesktopApplication::OnRender();
			ImGuiRender();

#ifdef XEN_DEVICE_DESKTOP
			m_Context->SwapBuffers();
			window->Update();
#endif
		}

		for (int i = stack->GetCount(); i >= 1; i--)
		{
			stack->GetLayer(i)->OnDetach();
		}
#ifdef XEN_DEVICE_DESKTOP
		m_Context->DestroyContext();
#endif
	}
}
