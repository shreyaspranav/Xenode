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
#include <core/scene/EditorCameraController.h>

#include <core/app/Profiler.h>

class TestLayer : public Xen::Layer
{
public:
	TestLayer() {}
	virtual ~TestLayer() {}

	void OnAttach() override
	{
		m_Camera = std::make_shared<Xen::Camera>(Xen::CameraType::Perspective, 
			Xen::DesktopApplication::GetWindow()->GetFrameBufferWidth(), 
			Xen::DesktopApplication::GetWindow()->GetFrameBufferHeight());

		input = Xen::Input::GetInputInterface();
		input->SetWindow(Xen::DesktopApplication::GetWindow());
		Xen::Renderer2D::Init();

		cam_control = Xen::EditorCameraController(input);

		texture = Xen::Texture2D::CreateTexture2D("assets/textures/Vulkan.png", true);
		texture->LoadTexture();

		texture1 = Xen::Texture2D::CreateTexture2D("assets/textures/opengl.png", true);
		texture1->LoadTexture();
	}

	void OnDetach() override
	{

	}

	void OnUpdate(double timestep) override
	{
		XEN_PROFILE_FN();

		cam_pos = cam_control.GetCameraPosition();

		bool active = true;
		cam_control.Update(&active);

		Xen::RenderCommand::Clear();
		Xen::RenderCommand::SetClearColor(Xen::Color(0.13f));

		if (zoom_iterations > 0)
		{
			//m_Camera->SetScale(m_Camera->GetScale().z += scroll_direction * 0.03f);
			zoom_iterations--;
		}

		//if (input->IsKeyPressed(Xen::KeyCode::KEY_A))
		//	cam_pos.x -= 0.06 * timestep;
		//if (input->IsKeyPressed(Xen::KeyCode::KEY_D))
		//	cam_pos.x += 0.06 * timestep;
		//if (input->IsKeyPressed(Xen::KeyCode::KEY_W))
		//	cam_pos.y += 0.06 * timestep;
		//if (input->IsKeyPressed(Xen::KeyCode::KEY_S))
		//	cam_pos.y -= 0.06 * timestep;

		m_Camera->LookAtPoint(cam_control.GetFocalPoint());
		m_Camera->SetPosition(cam_pos);
		m_Camera->Update(1);

		Xen::Renderer2D::BeginScene(m_Camera);
		for (int i = 0; i < quads; i++) {
			for (int j = 0; j < quads; j++) {
				//Xen::Renderer2D::DrawClearQuad(Xen::Vec3((float)i, (float)j, 0.0f), Xen::Vec3(0.0f, 0.0f, 45.0f), Xen::Vec2(0.9f), color);
			}
		}

		//Xen::Renderer2D::DrawClearQuad(Xen::Vec3((float)0.0f, (float)0.0f, -1.0f), Xen::Vec3(0.0f, 0.0f, 0.0f), Xen::Vec3(1.0f), color);

		//Xen::Renderer2D::DrawClearCircle(Xen::Vec3(0.0f, 0.0f, -1.0f), Xen::Vec3(0.0f, 0.0f, 0.0f), Xen::Vec2(1.0f), Xen::Color(1.0f, 0.0f, 1.0f, 1.0f), thickness, innerfade, outerfade);
		//Xen::Renderer2D::DrawTexturedQuad(texture, Xen::Vec3(0.0f, 0.0f, 1.0f), Xen::Vec3(0.0f, 0.0f, 0.0f), Xen::Vec2(2.4f, 1.0f), {1.0f, 0.0f, 0.0f, 1.0f});
		//Xen::Renderer2D::DrawTexturedQuad(texture1, Xen::Vec3(3.0f, 0.0f, 1.0f), Xen::Vec3(0.0f, 0.0f, 0.0f), Xen::Vec2(2.4f, 1.0f), Xen::Color(1.0f));

		Xen::Renderer2D::DrawClearTriangle({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, z_Rotation}, scale, color);


		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				//Xen::Renderer2D::DrawClearCircle(Xen::Vec3((float)i, (float)j, -0.2f), Xen::Vec3(0.0f, 0.0f, 0.0f), Xen::Vec3(0.5f, 0.5f, 1.0f), Xen::Color(1.0f, 0.4f, 0.8f, 1.0f), 0.4f, 1.0f, 0.03f);
			}
		}

		Xen::Renderer2D::EndScene();
	}

	void OnImGuiUpdate() override
	{
		ImGui::Begin("Test Window");

		//ImGui::SliderInt("Quads", &quads, 1, 201);
		//ImGui::SliderFloat("Thickness", &thickness, 0.0f, 1.0f);
		//ImGui::SliderFloat("Inner Fade", &innerfade, 0.0f, 1.0f);
		//ImGui::SliderFloat("Outer Fade", &outerfade, 0.0f, 1.0f);

		ImGui::SliderFloat("Triangle Z rotation", &z_Rotation, 0.0f, 180.0f);
		ImGui::SliderFloat("Scale X Triangle", &scale.x, 0.1f, 5.0f);
		ImGui::SliderFloat("Scale Y Triangle", &scale.y, 0.1f, 5.0f);
		//ImGui::ColorEdit4("Triangle Color", color.GetVec());

		ImGui::End();
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

	Xen::EditorCameraController cam_control;

	int32_t quads = 1;

	Xen::Ref<Xen::Texture2D> texture;
	Xen::Ref<Xen::Texture2D> texture1;

	float thickness = 1.0f;
	float innerfade = 0.0f;
	float outerfade = 0.0f;

	float z_Rotation = 0.0f;

	Xen::Vec2 scale = { 1.0f, 1.0f };

	//float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	Xen::Color color[3] = {
		Xen::Color(1.0f, 0.0f, 0.0f, 1.0f),
		Xen::Color(0.0f, 1.0f, 0.0f, 1.0f),
		Xen::Color(0.0f, 0.0f, 1.0f, 1.0f),
	};
};