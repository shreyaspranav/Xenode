#pragma once
#ifdef XEN_ENABLE_PROFILING

#include <optick.h>

#define XEN_START_PROFILER(...)				OPTICK_START_CAPTURE(__VA_ARGS__)
#define XEN_STOP_PROFILER(...)				OPTICK_STOP_CAPTURE(__VA_ARGS__)
#define XEN_SAVE_PROFILER_CAPTURE(...)		OPTICK_SAVE_CAPTURE(__VA_ARGS__)

#define XEN_PROFILER_FRAME(...)				OPTICK_FRAME(__VA_ARGS__)
#define XEN_PROFILE_FN(...)					OPTICK_EVENT(__VA_ARGS__)
#define XEN_PROFILE_GPU(...)				OPTICK_GPU_EVENT(__VA_ARGS__)

#define  XEN_PROFILER_TAG(NAME, ...)		OPTICK_TAG(NAME, __VA_ARGS__)

#else

#define XEN_START_PROFILER(...)				
#define XEN_STOP_PROFILER(...)				
#define XEN_SAVE_PROFILER_CAPTURE(...)	

#define XEN_PROFILER_FRAME(...)				
#define XEN_PROFILE_FN(...)					
#define XEN_PROFILE_GPU(...)

#define  XEN_PROFILER_TAG(...)				

#endif // XEN_PROFILING_ENABLE