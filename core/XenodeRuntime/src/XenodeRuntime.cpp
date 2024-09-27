#define XEN_INCLUDE_ENTRY_POINT
#include <Xenode.h>

#include <project/ProjectManager.h>

#include "MainRuntimeLayer.h"

// TODO: This is temporary, find a better solution
#define RUNTIME false

class XenodeRuntimeApplication : public Xen::DesktopGameApplication
{
public:
	XenodeRuntimeApplication()
		:Xen::DesktopGameApplication(RUNTIME) {}
	virtual ~XenodeRuntimeApplication()       {}
	
	void OnCreate() override
	{
		Xen::DesktopGameApplication::OnCreate();

		// Setup the gameProperties:
		gameProperties.windowWidth = 1600;
		gameProperties.windowHeight = 900;
		gameProperties.windowVsync = true;

		gameProperties.fullScreenMonitorIndex = 0;

		// Load a project into the ProjectManager
		std::filesystem::path relProjectPath = "../../resources/projects/default_project/default_project.xenproject";
		Xen::Ref<Xen::Project> loadedProject = Xen::ProjectManager::LoadProject(relProjectPath);

		if (loadedProject)
			gameProperties.windowTitle = loadedProject->GetProjectProperties().name;
		else
			gameProperties.windowTitle = "Xenode Runtime: Project failed to load!";
	}

	void OnStart() override 
	{
		Xen::DesktopGameApplication::OnStart();

		Xen::Ref<Xen::Layer> mainRuntimeLayer = std::make_shared<MainRuntimeLayer>();
		DesktopGameApplication::PushLayer(mainRuntimeLayer);
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
};

Xen::DesktopGameApplication* Xen::CreateApplication() { return new XenodeRuntimeApplication(); }