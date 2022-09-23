#include "pch"
#include "GameApplication.h"
#include "Log.h"
#include "LayerStack.h"

#define GLAD_GL_IMPLEMENTATION

#include "Window.h"
#include "Input.h"
#include "Monitor.h"
#include "Timer.h"

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

		dispatcher.SetEventCallbackFn(EventType::MouseEnterEvent, std::bind(&GameApplication::OnMouseEnterEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::MouseMoveEvent, std::bind(&GameApplication::OnMouseMoveEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::MouseButtonPressEvent, std::bind(&GameApplication::OnMouseButtonPressEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::MouseButtonReleaseEvent, std::bind(&GameApplication::OnMouseButtonReleaseEvent, this, std::placeholders::_1));
		dispatcher.SetEventCallbackFn(EventType::MouseScrollEvent, std::bind(&GameApplication::OnMouseScrollEvent, this, std::placeholders::_1));

		stack = LayerStack(20);
	}

	GameApplication::~GameApplication()
	{
		
	}

	void GameApplication::PushLayer(const Ref<Layer>& layer)				{ stack.PushLayer(layer); }
	void GameApplication::PushLayer(const Ref<Layer>& layer, uint8_t loc)	{ stack.PushLayer(layer, loc); }
	void GameApplication::PopLayer()										{ stack.PopLayer(); }
	void GameApplication::PopLayer(uint8_t loc)								{ stack.PopLayer(loc); }

	void GameApplication::OnCreate()
	{
	}
	void GameApplication::OnStart()
	{
	}
	void GameApplication::OnUpdate(double timestep)
	{
	}

	void GameApplication::OnWindowMoveEvent(Event& event)
	{
		WindowMoveEvent& evt = static_cast<WindowMoveEvent&>(event);
		for (const Ref<Layer>& layer : stack.GetLayerStack()) { layer->OnWindowMoveEvent(evt); }
	}

	void GameApplication::OnWindowResizeEvent(Event& event)
	{
		WindowResizeEvent& evt = static_cast<WindowResizeEvent&>(event);
		for (const Ref<Layer>& layer : stack.GetLayerStack()) { layer->OnWindowResizeEvent(evt); }
	}

	void GameApplication::OnWindowCloseEvent(Event& event)
	{
		WindowCloseEvent& evt = static_cast<WindowCloseEvent&>(event);
		for (const Ref<Layer>& layer : stack.GetLayerStack()) { layer->OnWindowCloseEvent(evt); }
		is_Running = 0;
	}

	void GameApplication::OnWindowFocusEvent(Event& event)
	{
		WindowFocusEvent& evt = static_cast<WindowFocusEvent&>(event);
		for (const Ref<Layer>& layer : stack.GetLayerStack()) { layer->OnWindowFocusEvent(evt); }
	}

	void GameApplication::OnWindowMinimizeEvent(Event& event)
	{
		WindowMinimizeEvent& evt = static_cast<WindowMinimizeEvent&>(event);
		for (const Ref<Layer>& layer : stack.GetLayerStack()) { layer->OnWindowMinimizeEvent(evt); }
	}

	void GameApplication::OnWindowMaximizeEvent(Event& event)
	{
		WindowMaximizeEvent& evt = static_cast<WindowMaximizeEvent&>(event);
		for (const Ref<Layer>& layer : stack.GetLayerStack()) { layer->OnWindowMaximizeEvent(evt); }
	}

	void GameApplication::OnKeyPressEvent(Event& event)
	{
		KeyPressEvent& evt = static_cast<KeyPressEvent&>(event);
		for (const Ref<Layer>& layer : stack.GetLayerStack()) { layer->OnKeyPressEvent(evt); }
	}

	void GameApplication::OnKeyReleaseEvent(Event& event)
	{
		KeyReleaseEvent& evt = static_cast<KeyReleaseEvent&>(event);
		for (const Ref<Layer>& layer : stack.GetLayerStack()) { layer->OnKeyReleaseEvent(evt); }
	}
	void GameApplication::OnMouseEnterEvent(Event& event)
	{
		MouseEnterEvent& evt = static_cast<MouseEnterEvent&>(event);
		for (const Ref<Layer>& layer : stack.GetLayerStack()) { layer->OnMouseEnterEvent(evt); }
	}

	void GameApplication::OnMouseMoveEvent(Event& event)
	{
		MouseMoveEvent& evt = static_cast<MouseMoveEvent&>(event);
		for (const Ref<Layer>& layer : stack.GetLayerStack()) { layer->OnMouseMoveEvent(evt); }
	}

	void GameApplication::OnMouseButtonPressEvent(Event& event)
	{
		MouseButtonPressEvent& evt = static_cast<MouseButtonPressEvent&>(event);
		for (const Ref<Layer>& layer : stack.GetLayerStack()) { layer->OnMouseButtonPressEvent(evt); }
	}

	void GameApplication::OnMouseButtonReleaseEvent(Event& event)
	{
		MouseButtonReleaseEvent& evt = static_cast<MouseButtonReleaseEvent&>(event);
		for (const Ref<Layer>& layer : stack.GetLayerStack()) { layer->OnMouseButtonReleaseEvent(evt); }
	}

	void GameApplication::OnMouseScrollEvent(Event& event)
	{
		MouseScrollEvent& evt = static_cast<MouseScrollEvent&>(event);
		for (const Ref<Layer>& layer : stack.GetLayerStack()) { layer->OnMouseScrollEvent(evt); }
	}


	void GameApplication::Run()
	{
		WindowProps props;
		Scope<Window> window = Window::GetWindow(props);
		window->Create();
		window->SetupEventListeners(dispatcher);

		window->SetWindowIcon("assets/icons/window_icon.png");
		window->SetCursorIcon("assets/icons/cursor_icon.png", 10, 10);

		Scope<Input> input = Input::GetInputInterface();
		input->SetWindow(window);
		input->SetupInputListeners();

		XEN_ENGINE_LOG_INFO("{0}", Timer::GetTime());

		std::vector<Ref<Monitor>> m = Monitor::GetAvailableMonitors();

		//TRIGGER_BREAKPOINT;

		while (is_Running) {
			window->Update();
		}

	}
}
