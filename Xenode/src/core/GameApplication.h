#pragma once
#include <Core.h>

namespace Xen {
	class XEN_API GameApplication
	{
	public:
		void Run();
	};
	XEN_API GameApplication* CreateApplication();
}

