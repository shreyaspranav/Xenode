#include "MainRuntimeLayer.h"

MainRuntimeLayer::MainRuntimeLayer()
{

}

MainRuntimeLayer::~MainRuntimeLayer()
{

}

void MainRuntimeLayer::OnAttach()
{
	m_CurrentScene = std::make_shared<Xen::Scene>();
	Xen::Renderer2D::Init();
	m_CurrentScene->OnCreate();
	Xen::SceneSerializer::Deserialize(m_CurrentScene, "assets/2DPhysicsTest.xen");

	m_CurrentScene->OnViewportResize(Xen::DesktopApplication::GetWindow()->GetFrameBufferWidth(), Xen::DesktopApplication::GetWindow()->GetFrameBufferHeight());

	m_CurrentScene->OnRuntimeStart();
}

void MainRuntimeLayer::OnDetach()
{

}

void MainRuntimeLayer::OnUpdate(double timestep)
{
	m_CurrentScene->OnUpdateRuntime(timestep, false);
}

void MainRuntimeLayer::OnFixedUpdate()
{

}

void MainRuntimeLayer::OnRender()
{
	m_CurrentScene->OnRender(true);
}

void MainRuntimeLayer::OnWindowResizeEvent(Xen::WindowResizeEvent& event)
{
	m_CurrentScene->OnViewportResize(event.GetWidth(), event.GetHeight());
}
