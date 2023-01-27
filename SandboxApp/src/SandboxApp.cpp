#include <Xenode.h>
#include <core/app/EntryPoint.h>
#include "FlappyBird.h"

class ExampleLayer : public Xen::Layer
{
private:
	bool window_handle = 1;
	float my_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};

public:
	ExampleLayer()
	{
		
	}

	virtual ~ExampleLayer()
	{

	}

	void OnAttach() override
	{
		XEN_APP_LOG_INFO("ExampleLayer Attached");
	}
	void OnDetach() override
	{

	}
	void OnUpdate(double timestep) override
	{

	}
	
	void OnImGuiUpdate() override
	{
		
	}

	void OnMouseScrollEvent(Xen::MouseScrollEvent& event) override
	{
		event.handled = 1;
	}
};

class SandboxApp : public Xen::DesktopApplication
{
public:
	SandboxApp()
	{

	}
	~SandboxApp()
	{

	}

	void OnCreate() override
	{
		window_title = "Flappy Bird";
		imgui_render = false;
	}
	void OnStart() override
	{
		Xen::Ref<Xen::Layer> testLayer = std::make_shared<FlappyBirdLayer>();
		PushLayer(testLayer);
		PushLayer(std::make_shared<ExampleLayer>());
	}
	void OnUpdate(double timestep) override
	{

	}

	void OnRender() override
	{

	}

	void OnFixedUpdate() override
	{

	}

private:

};

Xen::DesktopApplication* Xen::CreateDesktopApplication() { return new SandboxApp(); }