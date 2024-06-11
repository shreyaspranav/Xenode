#include <Xenode.h>
#include <core/app/EntryPoint.h>

#include "LevelEditorLayer.h"

class XenEditorApp : public Xen::DesktopApplication
{
public:
	XenEditorApp() {}
	~XenEditorApp() {}

	void OnCreate() override
	{
		window_width = 1600;
		window_height = 900;

		window_title = "Xen Editor";

		imgui_always_render = true;
		fullscreen_monitor = 0; // No fullscreen

		vsync = false;
	}

	void OnStart() override
	{
		PushLayer(std::make_shared<LevelEditorLayer>());
	}

	void OnUpdate(double timestep) override
	{

	}

	void OnFixedUpdate() override
	{

	}

	void OnRender() override
	{

	}

private:
};

Xen::DesktopApplication* Xen::CreateDesktopApplication() { return new XenEditorApp(); }