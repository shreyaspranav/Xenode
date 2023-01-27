#include <Xenode.h>
#include <core/app/EntryPoint.h>

#include "EditorLayer.h"

class XenEditorApp : public Xen::DesktopApplication
{
public:
	XenEditorApp() {}
	~XenEditorApp() {}

	void OnCreate() override
	{
		window_width = 1280;
		window_height = 720;

		window_title = "Xen Editor";

		imgui_always_render = 1;
		fullscreen_monitor = 0;

		vsync = 0;
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