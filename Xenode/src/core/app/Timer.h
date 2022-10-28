#pragma once

#include <Core.h>

namespace Xen {
	class XEN_API Timer
	{
	public:
		inline static double GetTimeS() { time_t a;  return time(&a); }// WTF??
		inline static double GetTimeMS() { time_t a; return time(&a) * 1000.0; }// WTF??
	};
}