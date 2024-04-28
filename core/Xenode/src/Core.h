#pragma once
#include <pch/pch>

#ifdef XEN_BUILD_LIB
	#define XEN_API
#endif
#ifdef XEN_BUILD_EXE
	#define XEN_API
#endif

#ifdef XEN_PLATFORM_ANDROID
	#define XEN_API
#endif

#define NO_PROBLEM 0
#define BIT(x) 1 << x

#ifdef XEN_PLATFORM_WINDOWS
	#define TRIGGER_BREAKPOINT __debugbreak()
#elif XEN_PLATFORM_LINUX
	#include <csignal>
	#define TRIGGER_BREAKPOINT std::raise(SIGTRAP)
#elif XEN_PLATFORM_ANDROID
	#include <android/log.h>
	#define TRIGGER_BREAKPOINT __android_log_assert("Triggered Breakpoint! ", "XENODE: ", NULL)
#endif

typedef size_t Size;

namespace Xen {

	// Custom Data types:

	// A 'Scope' Smart pointer
	template<typename T>
	using Scope = std::unique_ptr<T>;

	// A 'Reference' Smart Pointer
	template<typename T>
	using Ref = std::shared_ptr<T>;

	// A map and a unordered_map
	template<typename T, typename V>
	using UnorderedMap = std::unordered_map<T, V>;

	template<typename T, typename V>
	using Map = std::map<T, V>;
}


// TEMP: 
#define XEN_ENABLE_DEBUG_RENDERER