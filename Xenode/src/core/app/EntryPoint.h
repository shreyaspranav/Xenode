#pragma once
#include <Core.h>
#include "GameApplication.h"

#include "Log.h"
#include <Windows.h>

extern Xen::GameApplication* Xen::CreateApplication();

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
	XEN_INIT_LOGGER

	auto app = Xen::CreateApplication();
	app->Run();
	delete app;

	return 0;
}
#endif // XEN_PRODUCTION