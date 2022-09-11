#include "pch"
#include "GameApplication.h"
#include "Log.h"
#include "LayerStack.h"

namespace Xen {

	void GameApplication::Run()
	{
		XEN_ENGINE_LOG_ERROR_SEVERE("Hello!!");
		XEN_ENGINE_LOG_ERROR("Hello!! {0}", 32);
		XEN_ENGINE_LOG_WARN("Hello!!");
		XEN_ENGINE_LOG_INFO("Hello!!");
		XEN_ENGINE_LOG_TRACE("Hello!!");

		LayerStack s(100);
		s.PushLayer(2);
		s.PushLayer(4);
		s.PushLayer(5);
		s.PushLayer(5);
		s.PushLayer(33452);
		s.PushLayer(645);
		s.PushLayer(554);
		s.PushLayer(55);
		s.PushLayer(523);
		s.PushLayer(5);
		s.PushLayer(4325);
		s.PushLayer(255);
		s.PushLayer(52432423);

		s.PushLayer(69696969);
		s.PushLayer(420, 3);
		s.PushLayer(420, 101);
		s.PopLayer(2);
		s.PopLayer(7);
		s.PopLayer();
		s.Te();

		TRIGGER_BREAKPOINT;

	}
}
