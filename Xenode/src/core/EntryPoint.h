#pragma once
#include <Core.h>
#include <core/GameApplication.h>

extern Xen::GameApplication* Xen::CreateApplication();

int main()
{
	auto app = Xen::CreateApplication();
	app->Run();
	delete app;
	return 0;
}