#include "pch"
#include "GameApplication.h"
#include "Log.h"
#include "LayerStack.h"
#include "Input.h"
#include "Monitor.h"
#include "Timer.h"

#include <imgui/ImGuiLayer.h>

// Temporary Test Files!
#include "test/TestLayer.h"

namespace Xen {

	GameApplication::GameApplication()
	{
		is_Running = 1;
		dispatcher.SetEventCallbackFn(EventType::WindowMoveEvent, std::bind(&GameApplication::OnWindowMoveEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::WindowResizeEvent, std::bind(&GameApplication::OnWindowResizeEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::WindowCloseEvent, std::bind(&GameApplication::OnWindowCloseEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::WindowFocusEvent, std::bind(&GameApplication::OnWindowFocusEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::WindowMinimizeEvent, std::bind(&GameApplication::OnWindowMinimizeEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::WindowMaximizeEvent, std::bind(&GameApplication::OnWindowMaximizeEvent, this, std::placeholders::_1));

		dispatcher.SetEventCallbackFn(EventType::KeyPressEvent, std::bind(&GameApplication::OnKeyPressEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::KeyReleaseEvent, std::bind(&GameApplication::OnKeyReleaseEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::CharEnterEvent, std::bind(&GameApplication::OnCharEnterEvent, this, std::placeholders::_1));

		dispatcher.SetEventCallbackFn(EventType::MouseEnterEvent, std::bind(&GameApplication::OnMouseEnterEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::MouseMoveEvent, std::bind(&GameApplication::OnMouseMoveEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::MouseButtonPressEvent, std::bind(&GameApplication::OnMouseButtonPressEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::MouseButtonReleaseEvent, std::bind(&GameApplication::OnMouseButtonReleaseEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::MouseScrollEvent, std::bind(&GameApplication::OnMouseScrollEvent, this, std::placeholders::_1));

		stack = std::make_unique<LayerStack>(20);

		window_width = 1600;
		window_height = 900;
		window_title = "Xenode Application";
		vsync = 1;
		resizable = 1;
		fullscreen_monitor = 0;

		imgui_render = 0;

		m_Api = GraphicsAPI::XEN_OPENGL_API;
	}

	GameApplication::~GameApplication()
	{
		delete m_Context;
	}

	void GameApplication::PushLayer(const Ref<Layer>& layer) { stack->PushLayer(layer); }
	void GameApplication::PushLayer(const Ref<Layer>& layer, uint8_t loc) { stack->PushLayer(layer, loc); }
	void GameApplication::PopLayer() { stack->PopLayer(); }
	void GameApplication::PopLayer(uint8_t loc) { stack->PopLayer(loc); }

	void GameApplication::OnCreate()
	{
		OnCreate();
	}
	void GameApplication::OnStart()
	{
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

		Ref<Layer> testLayer = std::make_shared<TestLayer>();
		PushLayer(testLayer);

		m_ImGuiLayer = std::make_shared<ImGuiLayer>();
		m_ImGuiLayer->SetWindow(window);
		PushLayer(m_ImGuiLayer);

		OnStart();
	}
	void GameApplication::OnUpdate(double timestep)
	{
		OnUpdate(timestep);
		window->Update();

		for(int i = stack->GetCount(); i >=1; i--)
			stack->GetLayer(i)->OnUpdate(timestep);

		ImGuiRender();

		m_Context->SwapBuffers();
	}

	void* GameApplication::GetNativeWindow()
	{
		return window->GetNativeWindow();
	}

	void GameApplication::ImGuiRender()
	{
		if (imgui_render)
		{
			m_ImGuiLayer->Begin();
			for (int i = stack->GetCount(); i >= 1; i--)
				stack->GetLayer(i)->OnImGuiUpdate();
			m_ImGuiLayer->End();
		}
	}

	void GameApplication::OnWindowMoveEvent(Event& event)
	{
		WindowMoveEvent& evt = static_cast<WindowMoveEvent&>(event);
		for(int i = stack->GetCount(); i >=1; i--) { stack->GetLayer(i)->OnWindowMoveEvent(evt); }
	}

	void GameApplication::OnWindowResizeEvent(Event& event)
	{
		WindowResizeEvent& evt = static_cast<WindowResizeEvent&>(event);
		for(int i = stack->GetCount(); i >=1; i--) { stack->GetLayer(i)->OnWindowResizeEvent(evt); }
	}

	void GameApplication::OnWindowCloseEvent(Event& event)
	{
		WindowCloseEvent& evt = static_cast<WindowCloseEvent&>(event);
		for(int i = stack->GetCount(); i >=1; i--) { stack->GetLayer(i)->OnWindowCloseEvent(evt); }
		is_Running = 0;
	}

	void GameApplication::OnWindowFocusEvent(Event& event)
	{
		WindowFocusEvent& evt = static_cast<WindowFocusEvent&>(event);
		for(int i = stack->GetCount(); i >=1; i--) { stack->GetLayer(i)->OnWindowFocusEvent(evt); }
	}

	void GameApplication::OnWindowMinimizeEvent(Event& event)
	{
		WindowMinimizeEvent& evt = static_cast<WindowMinimizeEvent&>(event);
		for(int i = stack->GetCount(); i >=1; i--) { stack->GetLayer(i)->OnWindowMinimizeEvent(evt); }
	}

	void GameApplication::OnWindowMaximizeEvent(Event& event)
	{
		WindowMaximizeEvent& evt = static_cast<WindowMaximizeEvent&>(event);
		for(int i = stack->GetCount(); i >=1; i--) { stack->GetLayer(i)->OnWindowMaximizeEvent(evt); }
	}

	void GameApplication::OnKeyPressEvent(Event& event)
	{
		KeyPressEvent& evt = static_cast<KeyPressEvent&>(event);

		if (evt.GetKey() == KeyCode::KEY_GRAVE_ACCENT && imgui_render == 0)
			imgui_render = 1;
		else if (evt.GetKey() == KeyCode::KEY_GRAVE_ACCENT && imgui_render == 1)
			imgui_render = 0;

		for(int i = stack->GetCount(); i >=1; i--) { stack->GetLayer(i)->OnKeyPressEvent(evt); }
	}

	void GameApplication::OnKeyReleaseEvent(Event& event)
	{
		KeyReleaseEvent& evt = static_cast<KeyReleaseEvent&>(event);
		for(int i = stack->GetCount(); i >=1; i--) { stack->GetLayer(i)->OnKeyReleaseEvent(evt); }
	}

	void GameApplication::OnCharEnterEvent(Event& event)
	{
		CharEnterEvent& evt = static_cast<CharEnterEvent&>(event);
		for(int i = stack->GetCount(); i >=1; i--) { stack->GetLayer(i)->OnCharEnterEvent(evt); }
	}

	void GameApplication::OnMouseEnterEvent(Event& event)
	{
		MouseEnterEvent& evt = static_cast<MouseEnterEvent&>(event);
		for(int i = stack->GetCount(); i >=1; i--) { stack->GetLayer(i)->OnMouseEnterEvent(evt); }
	}

	void GameApplication::OnMouseMoveEvent(Event& event)
	{
		MouseMoveEvent& evt = static_cast<MouseMoveEvent&>(event);
		for(int i = stack->GetCount(); i >=1; i--) { stack->GetLayer(i)->OnMouseMoveEvent(evt); }
	}

	void GameApplication::OnMouseButtonPressEvent(Event& event)
	{
		MouseButtonPressEvent& evt = static_cast<MouseButtonPressEvent&>(event);
		for(int i = stack->GetCount(); i >=1; i--) { stack->GetLayer(i)->OnMouseButtonPressEvent(evt); }
	}

	void GameApplication::OnMouseButtonReleaseEvent(Event& event)
	{
		MouseButtonReleaseEvent& evt = static_cast<MouseButtonReleaseEvent&>(event);
		for(int i = stack->GetCount(); i >=1; i--) { stack->GetLayer(i)->OnMouseButtonReleaseEvent(evt); }
	}

	void GameApplication::OnMouseScrollEvent(Event& event)
	{
		MouseScrollEvent& evt = static_cast<MouseScrollEvent&>(event);
		for(int i = stack->GetCount(); i >=1; i--) { stack->GetLayer(i)->OnMouseScrollEvent(evt); }
	}


	void GameApplication::Run()
	{
		GameApplication::OnCreate();
		GameApplication::OnStart();

		double a = Timer::GetTimeMS();
		while (is_Running) {
			double b = Timer::GetTimeMS();
			GameApplication::OnUpdate(a - b);
			a = Timer::GetTimeMS();
			//XEN_ENGINE_LOG_INFO(a - b);
		}

		for(int i = stack->GetCount(); i >=1; i--)
		{
			stack->GetLayer(i)->OnDetach();
		}

		m_Context->DestroyContext();
	}
}
