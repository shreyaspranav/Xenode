#include <Xenode.h>
#include <core/app/EntryPoint.h>

#include <entt.hpp>

#include "EditorLayer.h"

class XenEditorApp : public Xen::GameApplication
{
public:
	XenEditorApp() {}
	~XenEditorApp() {}

	void OnCreate() override
	{
		window_width = 1600;
		window_height = 900;

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

Xen::GameApplication* Xen::CreateApplication() { return new XenEditorApp(); }