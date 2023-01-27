#include <Xenode.h>
#include <EntryPoint.h>

class ExampleLayer : public Xen::Layer
{
private:
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
		XEN_APP_LOG_INFO("ExampleLayer Detached");
	}
	void OnUpdate(double timestep) override
	{
		XEN_APP_LOG_INFO("ExampleLayer Update: {0}", timestep);
	}
};

class SandboxAppAndroid : public Xen::MobileApplication
{
public:
	SandboxAppAndroid()
	{

	}
	~SandboxAppAndroid()
	{

	}

	void OnCreate() override
	{
		XEN_APP_LOG_INFO("SandboxApp Created");
		PushLayer(std::make_shared<ExampleLayer>());
	}
	void OnStart() override
	{
		XEN_APP_LOG_INFO("SandboxApp Started");
	}
	void OnUpdate(double timestep) override
	{
		XEN_APP_LOG_INFO("SandboxApp Updated! {0}", timestep);
	}

	void OnRender() override
	{
		XEN_APP_LOG_INFO("SandboxApp Render!");
	}

	void OnFixedUpdate() override
	{
		XEN_APP_LOG_INFO("SandboxApp FixedUpdated!");
	}

private:

};

Xen::MobileApplication* Xen::CreateMobileApplication() { return new SandboxAppAndroid(); }

//END_INCLUDE(all)