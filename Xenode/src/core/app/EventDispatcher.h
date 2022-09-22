#pragma once

#include <Core.h>
#include "Events.h"

namespace Xen {

	template<typename T>
	struct EventFn
	{
		EventType type;
		std::function<void(T&)> fn;
	};

	class XEN_API EventDispatcher
	{
	public:
		EventDispatcher();
		~EventDispatcher();

		using EventCallbackFn = std::function<void(Event&)>;

		void SetEventCallbackFn(EventType type, EventCallbackFn&& fn);
		void PostEvent(Event& event);

	private:
		std::map<EventType, EventCallbackFn> m_EventFns;
	};
}

