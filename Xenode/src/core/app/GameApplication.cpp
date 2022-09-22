#include "pch"
#include "GameApplication.h"
#include "Log.h"
#include "LayerStack.h"

#define GLAD_GL_IMPLEMENTATION

#include "Window.h"
#include "Input.h"
#include "Monitor.h"

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

	}

	GameApplication::~GameApplication()
	{
		
	}

	void GameApplication::OnWindowMoveEvent(Event& event)
	{
		WindowMoveEvent& evt = static_cast<WindowMoveEvent&>(event);
		XEN_ENGINE_LOG_WARN("{0}", evt.ToString());
	}

	void GameApplication::OnWindowResizeEvent(Event& event)
	{
		WindowResizeEvent& evt = static_cast<WindowResizeEvent&>(event);
		XEN_ENGINE_LOG_WARN("{0}", evt.ToString());
	}

	void GameApplication::OnWindowCloseEvent(Event& event)
	{
		WindowCloseEvent& evt = static_cast<WindowCloseEvent&>(event);
		XEN_ENGINE_LOG_WARN("{0}", evt.ToString());
		is_Running = 0;
	}

	void GameApplication::OnWindowFocusEvent(Event& event)
	{
		WindowFocusEvent& evt = static_cast<WindowFocusEvent&>(event);
		XEN_ENGINE_LOG_WARN("{0}", evt.ToString());
	}

	void GameApplication::OnWindowMinimizeEvent(Event& event)
	{
		WindowMinimizeEvent& evt = static_cast<WindowMinimizeEvent&>(event);
		XEN_ENGINE_LOG_WARN("{0}", evt.ToString());
	}

	void GameApplication::OnWindowMaximizeEvent(Event& event)
	{
		WindowMaximizeEvent& evt = static_cast<WindowMaximizeEvent&>(event);
		XEN_ENGINE_LOG_WARN("{0}", evt.ToString());
	}

	void GameApplication::OnKeyPressEvent(Event& event)
	{
		KeyPressEvent& evt = static_cast<KeyPressEvent&>(event);
		XEN_ENGINE_LOG_INFO("{0}", evt.ToString());
	}

	void GameApplication::OnKeyReleaseEvent(Event& event)
	{
		KeyReleaseEvent& evt = static_cast<KeyReleaseEvent&>(event);
		XEN_ENGINE_LOG_INFO("{0}", evt.ToString());
	}
	void GameApplication::OnMouseEnterEvent(Event& event)
	{
		MouseEnterEvent& evt = static_cast<MouseEnterEvent&>(event);
		XEN_ENGINE_LOG_ERROR("{0}", evt.ToString());
	}

	void GameApplication::OnMouseMoveEvent(Event& event)
	{
		MouseMoveEvent& evt = static_cast<MouseMoveEvent&>(event);
		XEN_ENGINE_LOG_ERROR("{0}", evt.ToString());
	}

	void GameApplication::OnMouseButtonPressEvent(Event& event)
	{
		MouseButtonPressEvent& evt = static_cast<MouseButtonPressEvent&>(event);
		XEN_ENGINE_LOG_ERROR("{0}", evt.ToString());
	}

	void GameApplication::OnMouseButtonReleaseEvent(Event& event)
	{
		MouseButtonReleaseEvent& evt = static_cast<MouseButtonReleaseEvent&>(event);
		XEN_ENGINE_LOG_ERROR("{0}", evt.ToString());
	}

	void GameApplication::OnMouseScrollEvent(Event& event)
	{
		MouseScrollEvent& evt = static_cast<MouseScrollEvent&>(event);
		XEN_ENGINE_LOG_ERROR("{0}", evt.ToString());
	}


	void GameApplication::Run()
	{
		WindowProps props;
		Scope<Window> window = Window::GetWindow(props);
		window->Create();
		window->SetupEventListeners(dispatcher);

		Scope<Input> input = Input::GetInputInterface();
		input->SetWindow(window);
		input->SetupInputListeners();

		std::vector<Ref<Monitor>> m = Monitor::GetAvailableMonitors();

		//TRIGGER_BREAKPOINT;

		while (is_Running) {
			window->Update();
		}

	}
}
