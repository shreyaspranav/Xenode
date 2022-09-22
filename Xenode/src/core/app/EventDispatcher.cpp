#include "pch"
#include "EventDispatcher.h"

namespace Xen {
	EventDispatcher::EventDispatcher()
	{
	}
	EventDispatcher::~EventDispatcher()
	{
	}
	void EventDispatcher::SetEventCallbackFn(EventType type, EventCallbackFn&& fn) { m_EventFns[type] = fn; }

	void EventDispatcher::PostEvent(Event& event)
	{
		EventType type = event.GetEventName();

		if (m_EventFns.find(type) == m_EventFns.end())
			return;

		auto&& event_fn = m_EventFns.at(type);
		event_fn(event);
	}
}
