#include "pch"

#ifdef XEN_DEVICE_DESKTOP

#include "DesktopGameApplication.h"

#include <core/app/Profiler.h>
#include <core/app/Log.h>
#include <core/app/Timer.h>
#include <core/app/EventDispatcher.h>
#include <core/app/input/KeyboardInput.h>
#include <core/app/input/MouseInput.h>

#include <core/renderer/RenderCommand.h>

namespace Xen 
{
	// A Game should only have one instance of GameApplication, therefore the "only" current instance
	static GameApplication* s_ApplicationInstance = nullptr;

	// Declared in GameApplication.h
	GameApplication* GetApplicationInstance() { return s_ApplicationInstance; }

	DesktopGameApplication::DesktopGameApplication()
	{
		s_ApplicationInstance = this;
		m_IsGameRunning = true;
	}

	DesktopGameApplication::~DesktopGameApplication()
	{
		s_ApplicationInstance = nullptr;
	}

	void DesktopGameApplication::PushLayer(const Ref<Layer>& layer)
	{
		m_LayerStack.push_back(layer);
		layer->OnAttach();

		XEN_ENGINE_LOG_WARN("Layer Pushed to the stack");
	}

	void DesktopGameApplication::PopLayer()
	{
		auto& lastLayer = *(m_LayerStack.begin() + m_LayerStack.size());
		lastLayer->OnDetach();

		m_LayerStack.pop_back();

		XEN_ENGINE_LOG_WARN("Layer popped out of the stack");
	}
	
	void DesktopGameApplication::OnCreate()
	{
		
	}
	
	void DesktopGameApplication::OnStart()
	{
		XEN_PROFILE_FN();

		WindowProps props;

		props.api = gameProperties.api;
		props.width = gameProperties.windowWidth;
		props.height = gameProperties.windowHeight;
		props.title = gameProperties.windowTitle;
		props.vsync = gameProperties.windowVsync;

		m_GameWindow = Window::GetWindow(props);
		m_GameWindow->Create();

		// New Input API:
		KeyboardInput::Init(m_GameWindow);
		MouseInput::Init(m_GameWindow);
		

		// Set the event callback function before any event occurs.
		m_GameWindow->SetEventCallbackFunction(XEN_BIND_FN(DesktopGameApplication::OnEvent));

		std::vector<Ref<Monitor>> monitors = Monitor::GetAvailableMonitors();
		uint8_t monitorCount = Monitor::GetMonitorCount();

		XEN_ENGINE_LOG_INFO("Monitor Count: {0}. Monitor List: -----------------------------------------", monitorCount);

		uint32_t count = 0;
		for (const auto& monitor : monitors)
		{
			uint32_t width = monitor->GetMonitorWidth();
			uint32_t height = monitor->GetMonitorHeight();
			uint32_t refreshRate = monitor->GetMonitorRefreshRate();

			uint8_t redBits = monitor->GetMonitorRedBits();
			uint8_t greenBits = monitor->GetMonitorGreenBits();
			uint8_t blueBits = monitor->GetMonitorBlueBits();

			XEN_ENGINE_LOG_INFO("Monitor {0}: {1}x{2}, {3}Hz {4}r {5}g {6}b",
				count++,
				width,
				height,
				refreshRate,
				redBits,
				greenBits,
				blueBits
			);
		}

		XEN_ENGINE_LOG_INFO("---------------------------------------------------------------------------", monitorCount);

		if (gameProperties.fullScreenMonitorIndex != 0)
			m_GameWindow->SetFullScreenMonitor(monitors[gameProperties.fullScreenMonitorIndex - 1]);

		// TODO: Find a better way that involves the asset pipeline:
		// window->SetWindowIcon("assets/icons/window_icon.png");
		// window->SetCursorIcon("assets/icons/cursor_icon.png", 14, 10);

		// Create the Graphics Context:
		m_GraphicsContext = GraphicsContext::CreateContext(m_GameWindow);
		m_GraphicsContext->Init();

		// Initialize RenderCommand(creates the instance of the graphics api's abstraction) 
		RenderCommand::Init();

		m_ImGuiLayer = std::make_shared<ImGuiLayer>();
		m_ImGuiLayer->SetWindow(m_GameWindow);
		PushLayer(m_ImGuiLayer);
	}
	
	void DesktopGameApplication::OnUpdate(float timestep)
	{
		for (const auto& layer : m_LayerStack) 
			layer->OnUpdate(timestep);
	}

	void DesktopGameApplication::OnFixedUpdate()
	{
		for (const auto& layer : m_LayerStack) 
			layer->OnFixedUpdate();
	}

	void DesktopGameApplication::OnRender()
	{
		for (const auto& layer : m_LayerStack) 
			layer->OnRender();
	}
	
	void DesktopGameApplication::OnExit()
	{
		for (const auto& layer : m_LayerStack)
			layer->OnDetach();
	}

	void DesktopGameApplication::OnEvent(Event& event)
	{
		EventDispatcher::Dispatch<WindowCloseEvent>(event, XEN_BIND_FN(DesktopGameApplication::OnWindowCloseEvent));
		// EventDispatcher::Dispatch<CharEnterEvent>(event, XEN_BIND_FN(DesktopGameApplication::OnCharEnterEvent));
		
		for (const auto& layer : m_LayerStack)
			layer->OnEvent(event);

		// XEN_ENGINE_LOG_TRACE("Event: {0}", event.ToString());
	}

	void DesktopGameApplication::OnWindowCloseEvent(WindowCloseEvent& windowCloseEvent)
	{
		m_IsGameRunning = false;
	}

	void DesktopGameApplication::OnCharEnterEvent(CharEnterEvent& charEnterEvent)
	{
		// if (charEnterEvent.GetChar() == KeyCode::KEY_GRAVE_ACCENT)
		// 	gameProperties.enableDebugRenderer = false;
	}

	void DesktopGameApplication::ImGuiRender()
	{
		if (gameProperties.enableDebugRenderer)
		{
			m_ImGuiLayer->Begin();
			for (const auto& layer : m_LayerStack)
				layer->OnImGuiUpdate();
			m_ImGuiLayer->End();
		}
	}

	// Main Game Loop: ---------------------------------------------------------------------
	void DesktopGameApplication::Run()
	{
		// IMPORTANT: Timestep is in milliseconds 

		XEN_START_PROFILER();

		const float MS_PER_UPDATE = (1.0f / 60.0f) * 1000.0f;

		OnCreate();
		OnStart();

		double timestep = 0.0;
		double lag = 0.0;

		while (m_IsGameRunning)
		{
			XEN_PROFILER_FRAME("MainThread");

			Timer timer;

			XEN_PROFILER_TAG("Timestep", (float)timestep);
			lag += timestep;

			while (lag >= MS_PER_UPDATE)
			{
				OnFixedUpdate();
				lag -= MS_PER_UPDATE;
			}

			OnUpdate(timestep);

			// Run this function in a different thread:
			OnRender();
			ImGuiRender();

			m_GraphicsContext->SwapBuffers();
			m_GameWindow->Update();

			timer.Stop();

			// time is in microseconds, convert to milliseconds
			timestep = (float)timer.GetElapedTime() / 1000.0f;
		}

		m_GraphicsContext->DestroyContext();
		OnExit();

		XEN_STOP_PROFILER();

		// TODO: Make sure that the date and time gets appended after the file name
		XEN_SAVE_PROFILER_CAPTURE("logs/profiler_capture.opt")
	}
}

#endif
