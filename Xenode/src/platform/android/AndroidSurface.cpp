#include "pch"
#include "AndroidSurface.h"

#ifdef XEN_PLATFORM_ANDROID
#include <android/looper.h>

namespace Xen {
	AndroidSurface::AndroidSurface(void* application_context) : m_ApplicationContext((android_app*)application_context)
	{}

	AndroidSurface::~AndroidSurface() { AndroidSurface::Shutdown(); }

	void AndroidSurface::Create() 
	{

	}
	void AndroidSurface::Update() 
	{
		int32_t result, events;
		android_poll_source* poll_source;

		while (result = ALooper_pollAll(-1, NULL, &events, (void**)&poll_source) >= 0)
		{
			if(poll_source != NULL)
				poll_source->process(m_ApplicationContext, poll_source);

			if (m_Application->destroyRequested) 
			{
				AndroidSurface::Shutdown();
				return;
			}
		}
	}
	void AndroidSurface::Shutdown() {}
}
#endif