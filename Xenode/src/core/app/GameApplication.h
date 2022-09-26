#pragma once
#include <Core.h>

#include "EventDispatcher.h"
#include "LayerStack.h"

#include "Window.h"

#include <core/renderer/GraphicsContext.h>

namespace Xen {
	class XEN_API GameApplication
	{
	private:
		EventDispatcher dispatcher;
		bool is_Running;
		Scope<LayerStack> stack;
		Scope<Window> window;
		GraphicsContext* m_Context;

	public:
		uint32_t window_width, window_height;
		std::string window_title;
		bool vsync;
		uint8_t fullscreen_monitor;

	public:
		GameApplication();
		virtual ~GameApplication();

		void Run();

		void PushLayer(const Ref<Layer>& layer);
		void PushLayer(const Ref<Layer>& layer, uint8_t loc);

		void PopLayer();
		void PopLayer(uint8_t loc);

		virtual void OnCreate();
		virtual void OnStart();
		virtual void OnUpdate(double timestep);

		// Events:
		void OnWindowMoveEvent(Event& event);
		void OnWindowResizeEvent(Event& event);
		void OnWindowCloseEvent(Event& event);
		void OnWindowFocusEvent(Event& event);
		void OnWindowMinimizeEvent(Event& event);
		void OnWindowMaximizeEvent(Event& event);

		void OnKeyPressEvent(Event& event);
		void OnKeyReleaseEvent(Event& event);

		void OnMouseEnterEvent(Event& event);
		void OnMouseMoveEvent(Event& event);
		void OnMouseButtonPressEvent(Event& event);
		void OnMouseButtonReleaseEvent(Event& event);
		void OnMouseScrollEvent(Event& event);
	};

	XEN_API GameApplication* CreateApplication();
}

