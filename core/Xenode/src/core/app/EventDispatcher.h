#pragma once

#include <Core.h>
#include "Events.h"

namespace Xen {
	using EventCallbackFn = std::function<void(Event&)>;
	
	class XEN_API EventDispatcher
	{
	public:
		template <typename EventClass, typename Function>
		static void Dispatch(Event& event, const Function& function)
		{
			if (event.GetEventType() == EventClass::GetStaticType())
				function(static_cast<EventClass&>(event));
		}
	};
}