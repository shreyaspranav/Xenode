#include "pch"
#include "Input.h"
#include "GameApplication.h"

#include <gfxapi/window/glfw/GLFW_input.h>

namespace Xen {
	Scope<Input> Input::GetInputInterface()
	{
		switch (GameApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_unique<GLFW_input>();

		case GraphicsAPI::XEN_VULKAN_API:
			return std::make_unique<GLFW_input>();

		case GraphicsAPI::XEN_DIRECT3D_API:
			return nullptr;

		case GraphicsAPI::XEN_OPENGLES_API:
			return nullptr;

		case GraphicsAPI::XEN_METAL_API:
			return nullptr;
		}
	}
}