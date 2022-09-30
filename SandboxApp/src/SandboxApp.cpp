#include <Xenode.h>

class ExampleLayer : public Xen::Layer
{
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

	void OnKeyPressEvent(Xen::KeyPressEvent& event) override
	{
		XEN_APP_LOG_WARN(event.ToString());
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
	}
	void OnUpdate(double timestep) override
	{

	}


private:

};

Xen::GameApplication* Xen::CreateApplication() { return new SandboxApp(); }