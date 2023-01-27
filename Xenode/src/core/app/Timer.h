#pragma once

#include <Core.h>

namespace Xen {
	class XEN_API Timer
	{
	public:
	#ifdef XEN_DEVICE_DESKTOP

		inline static double GetTimeS() { time_t a;  return time(&a); }// WTF??
		inline static double GetTimeMS() { time_t a; return time(&a) * 1000.0; }// WTF??

	#elif XEN_DEVICE_MOBILE	
	#ifdef XEN_PLATFORM_ANDROID

		inline static double GetTimeS() 
		{
			struct timespec res;
			clock_gettime(CLOCK_MONOTONIC, &res);
			return res.tv_sec + ((double)res.tv_nsec / 1000000.0);
		}
		inline static double GetTimeMS() 
		{ 
			struct timespec res;
			clock_gettime(CLOCK_MONOTONIC, &res);
			return (1000.0 * res.tv_sec) + ((double)res.tv_nsec / 1000000.0);
		}

	#endif // XEN_PLATFORM_ANDROID
	#endif // XEN_DEVICE_DESKTOP
	};
}