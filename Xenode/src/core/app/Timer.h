#pragma once

#include <Core.h>

namespace Xen {
	class XEN_API Timer
	{
	#ifdef XEN_DEVICE_DESKTOP
	public:
		Timer() { m_StartTime = std::chrono::high_resolution_clock::now(); }
		~Timer() {}

		void Reset() { m_StartTime = std::chrono::high_resolution_clock::now(); }

		void Stop() 
		{
			auto endTime = std::chrono::high_resolution_clock::now();

			uint64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch().count();
			uint64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();

			m_ElapsedTime = end - start;
		}

		uint64_t GetElapedTime() { return m_ElapsedTime; }

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
		uint64_t m_ElapsedTime = 0;

		inline static double GetTimeS() { time_t a;  return time(&a); }// WTF??
		inline static double GetTimeMS() { time_t a; return time(&a) * 1000.0; }// WTF??

	#elif XEN_DEVICE_MOBILE	
	#ifdef XEN_PLATFORM_ANDROID
	public:
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