#include "MainRuntimeLayer.h"

#include <core/scene/SceneRuntime.h>
#include <core/app/desktop/DesktopGameApplication.h>

#include <project/ProjectManager.h>
#include <scene/SceneSerializer.h>

MainRuntimeLayer::MainRuntimeLayer()
{

}

MainRuntimeLayer::~MainRuntimeLayer()
{

}

void MainRuntimeLayer::OnAttach()
{
	Xen::DesktopGameApplication* dGameApplication = (Xen::DesktopGameApplication*)Xen::GetApplicationInstance();
	uint32_t framebufferWidth = dGameApplication->GetGameWindow()->GetFrameBufferWidth();
	uint32_t framebufferHeight = dGameApplication->GetGameWindow()->GetFrameBufferHeight();

	Xen::SceneRuntime::Initialize(framebufferWidth, framebufferHeight);

	std::filesystem::path relScenePath = 
		Xen::ProjectManager::GetCurrentProjectPath() / 
		Xen::ProjectManager::GetCurrentProject()->GetProjectSettings().relStartScenePath;

	m_CurrentScene = std::make_shared<Xen::Scene>(Xen::SceneType::_2D);

	Xen::SceneSerializer::Deserialize(m_CurrentScene, relScenePath.string());
	Xen::SceneRuntime::SetActiveScene(m_CurrentScene);
	Xen::SceneRuntime::RuntimeBegin();

	// TODO: Kind of annoying, but fix this issue where the window needs to be "resized" at the beginning
	Xen::SceneRuntime::ResizeFrameBuffer(framebufferWidth, framebufferHeight);
}

void MainRuntimeLayer::OnDetach()
{
	Xen::SceneRuntime::RuntimeEnd();
	Xen::SceneRuntime::Finalize();
}

void MainRuntimeLayer::OnUpdate(float timestep)
{
	Xen::SceneSettings s;
	s.renderSource = Xen::RenderSource::RuntimeCamera;
	s.renderToGameWindow = true;

	Xen::SceneRuntime::Begin(s);

	if (m_FirstIteration)
	{
		// Xen::SceneRuntime::RuntimeBegin();
		m_FirstIteration = false;
	}

	Xen::SceneRuntime::UpdateRuntime(timestep, false);
	Xen::SceneRuntime::End();
}

void MainRuntimeLayer::OnFixedUpdate()
{
	Xen::SceneRuntime::FixedUpdate();
}

void MainRuntimeLayer::OnRender()
{
	Xen::SceneRuntime::Render();
}

void MainRuntimeLayer::OnEvent(Xen::Event& event)
{
	Xen::EventDispatcher::Dispatch<Xen::WindowResizeEvent>(event, XEN_BIND_FN(MainRuntimeLayer::OnWindowResizeEvent));
}

void MainRuntimeLayer::OnWindowResizeEvent(Xen::WindowResizeEvent& event)
{
	Xen::SceneRuntime::ResizeFrameBuffer(event.GetWidth(), event.GetHeight());
}
