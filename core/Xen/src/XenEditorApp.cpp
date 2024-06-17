#include <Xenode.h>
#include <core/app/EntryPoint.h>

#include "LevelEditorLayer.h"
#include <project/ProjectManager.h>

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
#ifndef XEN_PRODUCTION
		// Load the default project in case of debug and release_debug builds:
		Xen::Ref<Xen::Project> p = Xen::ProjectManager::LoadProject("../../resources/projects/default_project/default_project.xenproject");
#else
		// TEMP: Implement project creation UI:
		XEN_ENGINE_LOG_ERROR("Project Creation UI Not implemented!");
		TRIGGER_BREAKPOINT;
#endif
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