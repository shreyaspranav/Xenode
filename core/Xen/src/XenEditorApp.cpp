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
		window_width = 1366;
		window_height = 768;

		window_title = "Xen Editor";

		imgui_always_render = true;
		fullscreen_monitor = 0; // No fullscreen

		vsync = true;
	}

	void OnStart() override
	{
		PushLayer(std::make_shared<EditorLayer>());
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