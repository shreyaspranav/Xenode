#include <Xenode.h>
#include <core/app/EntryPoint.h>

// TEMP:
#include <core/app/desktop/DesktopGameApplication.h>

class ExampleLayer : public Xen::Layer
{
private:
	bool window_handle = 1;
	float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};

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
		Xen::RenderCommand::Clear();
		Xen::RenderCommand::SetClearColor({ color[0], color[1], color[2], color[3] });
	}
	
	void OnImGuiUpdate() override
	{
		ImGui::ShowDemoWindow();

		ImGui::Begin("Background Color Test");
		ImGui::ColorPicker4("Background Color", color);
		ImGui::End();
	}
};

class SandboxApp : public Xen::DesktopGameApplication
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
		Xen::DesktopGameApplication::OnCreate();

		gameProperties.windowWidth = 1600;
		gameProperties.windowHeight = 900;
		gameProperties.windowTitle = "Application layer refactor test";
		gameProperties.windowVsync = true;
	}

	void OnStart() override
	{
		Xen::DesktopGameApplication::OnStart();
		PushLayer(std::make_shared<ExampleLayer>());
	}

	void OnUpdate(float timestep) override
	{
		Xen::DesktopGameApplication::OnUpdate(timestep);
	}

	void OnRender() override
	{
		Xen::DesktopGameApplication::OnRender();
	}

	void OnFixedUpdate() override
	{
		Xen::DesktopGameApplication::OnFixedUpdate();
	}
};

Xen::DesktopGameApplication* Xen::CreateApplication() { return new SandboxApp(); }