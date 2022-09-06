#pragma once

#include <Core.h>

namespace Xen {

	class XEN_API ActionListener
	{
	public:
		virtual void ActionPerformed() = 0;
	};
}