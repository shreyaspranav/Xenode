#pragma once
#include <Core.h>

#include <core/app/EventDispatcher.h>
#include "Monitor.h"

namespace Xen {

	struct WindowProps {
		uint16_t width, height;
		std::string title;
		bool vsync;

		WindowProps(const std::string& title = "Xenode Application",
						uint16_t width = 1024,
						uint16_t height = 576,
						bool vsync = 0) : title(title), width(width), height(height), vsync(vsync)
		{}
	};

	class XEN_API Window
	{
	public:
		virtual ~Window() = default;

		virtual void Create() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;

		virtual void SetWindowResolution(uint32_t width, uint32_t height) = 0;
		virtual void SetWindowMaxResolution(uint32_t width, uint32_t height) = 0;
		virtual void SetWindowMinResolution(uint32_t width, uint32_t height) = 0;
		virtual void SetWindowTitle(const std::string& title) = 0;

		virtual void SetWindowIcon(const std::string& icon_path) = 0;
		virtual void SetCursorIcon(const std::string& icon_path, uint16_t pointer_x, uint16_t pointer_y) = 0;

		virtual void MinimizeWindow() = 0;
		virtual void MaximizeWindow() = 0;

		virtual void ShowWindow(bool show) = 0;
		virtual void FocusWindow() = 0;

		inline virtual void SetVsync(bool enabled) = 0;

		inline virtual void* GetNativeWindow() const = 0;

		inline virtual uint32_t GetWidth() const = 0;
		inline virtual uint32_t GetHeight() const = 0;
		inline virtual const std::string& GetTitle() const = 0;
		inline virtual bool IsVsync() const = 0;

		virtual void SetFullScreenMonitor(const Ref<Monitor>& monitor) = 0;
		virtual void SetWindowed() = 0;

		virtual void SetupEventListeners(const EventDispatcher& dispacher) = 0;

		static Ref<Window> GetWindow(const WindowProps& props = WindowProps());
	};
}