#pragma once
#include <Core.h>
#include "Log.h"

#ifdef XEN_PLATFORM_WINDOWS
#include <Windows.h>
#endif // XEN_PLATFORM_WINDOWS


#ifdef XEN_DEVICE_DESKTOP

#include "DesktopApplication.h"
extern Xen::DesktopApplication* Xen::CreateDesktopApplication();

#ifdef XEN_PRODUCTION

	#ifdef XEN_PLATFORM_WINDOWS

		int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE HInstPrev, PSTR cmdline, int cmdshow)
		{
			XEN_INIT_LOGGER

			auto app = Xen::CreateApplication();
			app->Run();
			delete app;

			return 0;
		}

	#else
		int main()
		{
			XEN_INIT_LOGGER
		
			auto app = Xen::CreateApplication();
			app->Run();
			delete app;
		
			return 0;
		}

	#endif // XEN_PLATFORM_WINDOWS

#else

int main()
{
	XEN_INIT_LOGGER;

	auto app = Xen::CreateDesktopApplication();
	app->Run();
	delete app;

	return 0;
}
#endif // XEN_PRODUCTION
#endif // XEN_DEVICE_DESKTOP

#ifdef XEN_DEVICE_MOBILE
#ifdef XEN_PLATFORM_ANDROID

#include "android_native_app_glue.h"
#include "MobileApplication.h"
extern Xen::MobileApplication* Xen::CreateMobileApplication();

void android_main(struct android_app* state) 
{
	XEN_INIT_LOGGER;

	auto app = Xen::CreateMobileApplication();

	app->SetApplicationContext((void*)state);
	app->Run();

	delete app;
}

#endif // XEN_PLATFORM_ANDROID
#endif // XEN_DEVICE_MOBILE
