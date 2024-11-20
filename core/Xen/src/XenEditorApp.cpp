#define XEN_INCLUDE_ENTRY_POINT
#include <Xenode.h>

#include "LevelEditorLayer.h"
#include <project/ProjectManager.h>
#include <core/asset/AssetManagerUtil.h>

// TODO: This is temporary, find a better solution
#define RUNTIME false

class XenEditorApp : public Xen::DesktopGameApplication
{
public:
	XenEditorApp()
		:Xen::DesktopGameApplication(RUNTIME) {}
	~XenEditorApp() {}

	void OnCreate() override
	{
		Xen::DesktopGameApplication::OnCreate();

		gameProperties.windowWidth = 1600;
		gameProperties.windowHeight = 900;

		gameProperties.windowTitle= "Xen Editor";

		gameProperties.windowVsync = false;
	}

	void OnStart() override
	{
		Xen::DesktopGameApplication::OnStart();

#ifndef XEN_PRODUCTION
		// Load the default project in case  of debug and release_debug builds:
		std::string projectsPath(PROJECTS);
#if XEN_PLATFORM_WINDOWS
		std::replace(projectsPath.begin(), projectsPath.end(), '/', '\\');
#endif
		std::filesystem::path defaultProjectPath(projectsPath);
		defaultProjectPath = defaultProjectPath / "default_project" / "default_project.xenproject";
		// In future, this has to be an ABSOLUTE PATH.
		Xen::Ref<Xen::Project> p = Xen::ProjectManager::LoadProject(defaultProjectPath);
#else
		// TEMP: Implement project creation UI:
		XEN_ENGINE_LOG_ERROR("Project Creation UI Not implemented!");
		TRIGGER_BREAKPOINT;
#endif
		PushLayer(std::make_shared<LevelEditorLayer>());
	}

	void OnUpdate(float timestep) override
	{
		Xen::DesktopGameApplication::OnUpdate(timestep);
	}

	void OnFixedUpdate() override
	{
		Xen::DesktopGameApplication::OnFixedUpdate();
	}

	void OnRender() override
	{
		Xen::DesktopGameApplication::OnRender();
	}

private:
};

Xen::DesktopGameApplication* Xen::CreateApplication() { return new XenEditorApp(); }