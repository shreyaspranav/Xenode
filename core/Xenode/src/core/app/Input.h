#pragma once

#include <Core.h>
#include "EventDispatcher.h"
#include "Window.h"

namespace Xen {
	class XEN_API Input
	{
	public:
		virtual void SetNativeWindow(void* window) = 0;
		virtual void SetWindow(const Ref<Window>& window) = 0;

		virtual void SetupInputListeners() const = 0;

		virtual bool IsKeyPressed(KeyCode code) const = 0;
		virtual bool IsMouseButtonPressed(MouseKeyCode code) const = 0;

		virtual uint16_t GetMouseX() const = 0;
		virtual uint16_t GetMouseY() const = 0;

		static Scope<Input> GetInputInterface();
	};
}