#include <Xenode.h>

#include "Test.h"

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

	void OnKeyPressEvent(Xen::KeyPressEvent& event) override
	{
	}
};

class SandboxApp : public Xen::GameApplication
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

	}
	void OnStart() override
	{
		//Xen::Ref<Xen::ImGuiLayer> layer = std::make_shared<Xen::ImGuiLayer>();
		//layer->SetWindow(GetWindow());
		//PushLayer(layer);
		PushLayer(std::make_shared<ExampleLayer>());
		Xen::Ref<Xen::Layer> testLayer = std::make_shared<TestLayer>();
		PushLayer(testLayer);
	}
	void OnUpdate(double timestep) override
	{

	}


private:

};

Xen::GameApplication* Xen::CreateApplication() { return new SandboxApp(); }