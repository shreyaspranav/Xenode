#include "pch"
#include "GameApplication.h"
#include "Log.h"
#include "LayerStack.h"

#include <GLFW/glfw3.h>

namespace Xen {

	void GameApplication::Run()
	{
		XEN_ENGINE_LOG_ERROR_SEVERE("Hello!!");
		XEN_ENGINE_LOG_ERROR("Hello!! {0}", 32);
		XEN_ENGINE_LOG_WARN("Hello!!");
		XEN_ENGINE_LOG_INFO("Hello!!");
		XEN_ENGINE_LOG_TRACE("Hello!!");
		
		
		LayerStack s(100);
		int a = glfwInit();

		XEN_APP_LOG_INFO("{0}", a);
		TRIGGER_BREAKPOINT;

	}
}
