#include <Xenode.h>
#include <core/app/EntryPoint.h>

#include "MainRuntimeLayer.h"

class XenodeRuntimeApplication : public Xen::DesktopApplication
{
public:
	XenodeRuntimeApplication()
	{

	}
	virtual ~XenodeRuntimeApplication()
	{
		
	}
	
	void OnCreate() override
	{
		window_width = 1920;
		window_height = 1200;

		window_title = "Xenode Runtime";
		imgui_render = true;
		vsync = false;

		fullscreen_monitor = 1;
	}

	void OnStart() override 
	{
		Xen::Ref<Xen::Layer> mainRuntimeLayer = std::make_shared<MainRuntimeLayer>();
		DesktopApplication::PushLayer(mainRuntimeLayer);
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
};

Xen::DesktopApplication* Xen::CreateDesktopApplication() { return new XenodeRuntimeApplication(); }