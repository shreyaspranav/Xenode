// Documentation: ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Core.h is the file that is included to every header file in the engine, it contains code that is common to all the 
// header files in the engine.
// -----------------------------------------------------------------------------------------------------------------------------------------------
// Here are the list of compiler definitions defined by premake(build system) based on various factors:
// 
// Platform macros:
//		XEN_PLATFORM_WINDOWS		-> defined for windows systems
//		XEN_PLATFORM_LINUX			-> defined for linux systems
//		XEN_PLATFORM_ANDROID		-> defined for android systems
// 
//		XEN_DEVICE_DESKTOP			-> defined for desktop systems
//		XEN_DEVICE_MOBILE			-> defined for mobile systems
// 
// Build configurations:
//		XEN_DEBUG					-> "Debug" build
//		XEN_RELEASE					-> "Release_Debug" build, it is a debug build with some optimizations turned on
//		XEN_PRODUCTION				-> "Production" build, final build that will be distributed
// 
// Build Target:
//		XEN_BUILD_LIB				-> defined when built as a shared library
//		XEN_BUILD_EXE				-> defined when built as a executable(mostly used when other targets are using 
//									the header files in the engine core)
// 
// Logging: 
//		XEN_LOG_ON					-> Logging is enabled(Logging specific code will be compiled)
//		XEN_LOG_OFF					-> Logging is disabled(Logging specific code will not be compiled)
// 
// Profiling:
//		XEN_ENABLE_PROFILING		-> Enable profiling(optick)[will be enabled when projects are generated with --enable-profiling option]
// 
// Miscellaneous: 
//	"Common Resources" is a folder that contains resources might be common for a lot of projects.
//	Such as shaders, common textures, etc. 
//		COMMON_RESOURCES="..."		-> Relative path to the common resources folder
// 
//	"Editor Resources" is a folder that contains resources specific to the level editor
//		EDITOR_RESOURCES="..."		-> Relative path to the editor resources folder
//	
// TODO: In the future, the "common resources" should be packed into an asset pack and should be loaded into every project,
// and the "editor resources" should be packed into a "bin" folder or something convenient on the final build.
// Find a good approach to this in regarding the build configurations and decide what to do.
// 
//	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

#define XEN_BIND_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Xen {

	// Custom Data types:

	// A 'Scope' Smart pointer
	template<typename T>
	using Scope = std::unique_ptr<T>;

	// A 'Reference' Smart Pointer
	template<typename T>
	using Ref = std::shared_ptr<T>;

	// A Dynamic Array:
	template<typename T>
	using Vector = std::vector<T>;

	// A map and a unordered_map
	template<typename T, typename V>
	using UnorderedMap = std::unordered_map<T, V>;

	template<typename T, typename V>
	using Map = std::map<T, V>;
}


// TEMP: 
#define XEN_ENABLE_DEBUG_RENDERER