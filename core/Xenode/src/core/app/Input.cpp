#include "pch"
#include "Input.h"
#include "DesktopApplication.h"

#include <gfxapi/window/glfw/GLFW_input.h>

namespace Xen {
	Scope<Input> Input::GetInputInterface()
	{
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_unique<GLFW_input>();
		}
		return nullptr;
	}
}