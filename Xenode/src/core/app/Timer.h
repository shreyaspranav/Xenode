#pragma once

#include <Core.h>

namespace Xen {
	class XEN_API Timer
	{
	public:
		inline static double GetTime() { return std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count(); }
	};
}