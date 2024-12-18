#pragma once

#include <Core.h>
#include <core/app/desktop/Window.h>

namespace Xen {
	class XEN_API GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static GraphicsContext* CreateContext(const Ref<Window>& window);
		virtual void DestroyContext() = 0;
	};
}