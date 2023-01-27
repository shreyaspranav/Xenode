#pragma once
#include <pch/pch>

#ifdef XEN_PLATFORM_WINDOWS
  #ifdef XEN_BUILD_LIB

	#ifdef _MSC_VER
	  #define XEN_API
	#else
	  #error Visual Studio only accepted in Windows!
	#endif

  #endif
  #ifdef XEN_BUILD_EXE

	#ifdef _MSC_VER
	  #define XEN_API
	#else
	  #error Visual Studio only accepted in Windows!
	#endif
  #endif
#endif

#ifdef XEN_PLATFORM_LINUX
  #ifdef XEN_BUILD_LIB

	#ifdef __GNUG__
	  #define XEN_API __attribute__((visibility("default")))
	#else
	  #error GCC only accepted in Linux!
	#endif

  #endif
  #ifdef XEN_BUILD_EXEC

	#ifdef __GNUG__
	  #define XEN_API
	#else
	  #error GCC only accepted in Linux!
	#endif
  #endif
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

namespace Xen {
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;
}
