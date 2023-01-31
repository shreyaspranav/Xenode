#include "pch"
#include "Surface.h"

#ifdef XEN_DEVICE_MOBILE
#include <platform/android/AndroidSurface.h>

namespace Xen {
	Ref<Surface> Surface::GetSurface(void* application_context)
	{
	#ifdef XEN_PLATFORM_ANDROID
		return std::make_shared<AndroidSurface>(application_context);
	#endif
		return nullptr;
	}
}
#endif // XEN_DEVICE_MOBILE
