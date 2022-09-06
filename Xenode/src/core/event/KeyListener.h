#pragma once

#include "ActionListener.h"
#include <core/app/KeyCodes.h>

namespace Xen {
	struct KeyListenerData
	{
		KeyCode code;
		bool isRepeat;
	};

	class XEN_API KeyListener : public ActionListener
	{
		virtual void OnKeyPressed(const KeyListenerData& data) = 0;
		virtual void OnKeyReleased(const KeyListenerData& data) = 0;
	};
}