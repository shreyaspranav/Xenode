#pragma once
#include <Core.h>
#include "GameApplication.h"

#include "Log.h"

extern Xen::GameApplication* Xen::CreateApplication();

int main()
{
	XEN_INIT_LOGGER

	auto app = Xen::CreateApplication();
	app->Run();
	delete app;

	return 0;
}