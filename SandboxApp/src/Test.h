#pragma once
#include <Core.h>

#include <core/app/Log.h>
#include <core/app/Layer.h>
#include <core/app/DesktopApplication.h>
#include <core/app/Input.h>

#include <core/renderer/Buffer.h>
#include <core/renderer/VertexArray.h>
#include <core/renderer/Shader.h>
#include <core/renderer/Renderer2D.h>
#include <core/renderer/Camera.h>
#include <core/renderer/Texture.h>
#include <core/renderer/Structs.h>

#include <core/app/Profiler.h>

class TestLayer : public Xen::Layer
{
public:
	TestLayer() {}
	virtual ~TestLayer() {}

	void OnAttach() override
	{
		m_Camera = std::make_shared<Xen::Camera>(Xen::CameraType::Orthographic, 
			Xen::DesktopApplication::GetWindow()->GetFrameBufferWidth(), 
			Xen::DesktopApplication::GetWindow()->GetFrameBufferHeight());

		input = Xen::Input::GetInputInterface();
		input->SetWindow(Xen::DesktopApplication::GetWindow());
		Xen::Renderer2D::Init();
	}

	void OnDetach() override
	{

	}

	void OnUpdate(double timestep) override
	{
		XEN_PROFILE_FN();

		Xen::RenderCommand::Clear();
		Xen::RenderCommand::SetClearColor(Xen::Color(0.13f));

		if (zoom_iterations > 0)
		{
			m_Camera->SetScale(m_Camera->GetScale().z += scroll_direction * 0.03f);
			zoom_iterations--;
		}

		if (input->IsKeyPressed(Xen::KeyCode::KEY_A))
			cam_pos.x -= 0.06 * timestep;
		if (input->IsKeyPressed(Xen::KeyCode::KEY_D))
			cam_pos.x += 0.06 * timestep;
		if (input->IsKeyPressed(Xen::KeyCode::KEY_W))
			cam_pos.y += 0.06 * timestep;
		if (input->IsKeyPressed(Xen::KeyCode::KEY_S))
			cam_pos.y -= 0.06 * timestep;

		m_Camera->SetPosition(cam_pos);
		m_Camera->Update();

		Xen::Renderer2D::BeginScene(m_Camera);
		//for (int i = 0; i < 10; i++) {
		//	for (int j = 0; j < 10; j++) {
		//		Xen::Renderer2D::DrawClearQuad(Xen::Vec3((float)i, (float)j, 0.0f), Xen::Vec3(0.0f, 0.0f, 0.0f), Xen::Vec3(0.9f), Xen::Color(0.0f, 0.4f, 0.8f, 1.0f));
		//	}
		//}

		Xen::Renderer2D::DrawClearQuad(Xen::Vec3((float)0.0f, (float)0.0f, 0.0f), Xen::Vec3(0.0f, 0.0f, 0.0f), Xen::Vec3(1.0f), Xen::Color(0.0f, 0.4f, 0.8f, 1.0f));


		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				//Xen::Renderer2D::DrawClearCircle(Xen::Vec3((float)i, (float)j, -0.2f), Xen::Vec3(0.0f, 0.0f, 0.0f), Xen::Vec3(0.5f, 0.5f, 1.0f), Xen::Color(1.0f, 0.4f, 0.8f, 1.0f), 0.4f, 1.0f, 0.03f);
			}
		}

		Xen::Renderer2D::EndScene();
	}

	void OnImGuiUpdate() override
	{

	}

	void OnRender() override
	{
		Xen::Renderer2D::RenderFrame();
	}

	void OnWindowResizeEvent(Xen::WindowResizeEvent& evt) override
	{
		Xen::RenderCommand::OnWindowResize(evt.GetWidth(), evt.GetHeight());
		m_Camera->OnViewportResize(evt.GetWidth(), evt.GetHeight());
	}

	void OnMouseScrollEvent(Xen::MouseScrollEvent& evt) override
	{
		scroll_direction = (int)evt.GetYOffset();

		zoom_iterations = 20;
	}
private:
	Xen::Ref<Xen::Camera> m_Camera;
	Xen::Ref<Xen::Input> input;

	Xen::Vec3 cam_pos;
	float cam_zoom = 1.0f;
	uint32_t zoom_iterations = 0;
	int scroll_direction = 0;
};