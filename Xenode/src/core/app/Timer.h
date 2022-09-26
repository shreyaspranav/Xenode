#pragma once

#include <Core.h>

namespace Xen {
	class XEN_API Timer
	{
	public:
		inline static double GetTimeS()  { return std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count(); }// WTF??
		inline static double GetTimeMS() { return std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() * 1000.0; }// WTF??
	};
}