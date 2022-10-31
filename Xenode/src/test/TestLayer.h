#pragma once
#include <Core.h>

#include <core/app/Log.h>
#include <core/app/Layer.h>
#include <core/app/GameApplication.h>
#include <core/app/Input.h>

#include <core/renderer/Buffer.h>
#include <core/renderer/VertexArray.h>
#include <core/renderer/Shader.h>
#include <core/renderer/Renderer2D.h>
#include <core/renderer/OrthographicCamera.h>
#include <core/renderer/Texture.h>
#include <core/renderer/Primitives.h>

class TestLayer : public Xen::Layer
{
public:
	TestLayer() {}
	virtual ~TestLayer() {}

	void OnAttach() override
	{
		m_Camera = std::make_shared<Xen::OrthographicCamera>(Xen::GameApplication::GetWindow()->GetFrameBufferWidth(), Xen::GameApplication::GetWindow()->GetFrameBufferHeight());
		Xen::Renderer2D::Init();

		quad = Xen::_2D::Quad();
		input = Xen::Input::GetInputInterface();
		input->SetWindow(Xen::GameApplication::GetWindow());

		tex = Xen::Texture2D::CreateTexture2D("assets/textures/CheckerBoardTexture.png", 0);
		opengl_logo = Xen::Texture2D::CreateTexture2D("assets/textures/opengl.png", 1);
		vulkan_logo = Xen::Texture2D::CreateTexture2D("assets/textures/Vulkan.png", 1);
		opengl_logo->LoadTexture();
		vulkan_logo->LoadTexture();
		tex->LoadTexture();
		
	}

	void OnDetach() override
	{

	}

	void OnUpdate(double timestep) override
	{
		color_gradient_quad_color[2].r = a;
		color_gradient_quad_color[3].r = a;

		quad.CalculateVerticesAndIndices();
		if (input->IsKeyPressed(Xen::KeyCode::KEY_SPACE))
			cam_zoom += 1.0 * (timestep / 1.0f);
		if (input->IsKeyPressed(Xen::KeyCode::KEY_TAB))
			cam_zoom -= 1.0 * (timestep / 1.0f);
		if (input->IsKeyPressed(Xen::KeyCode::KEY_1))
			quad.scale.x += 10.0 * (timestep / 1.0f);
		if (input->IsKeyPressed(Xen::KeyCode::KEY_2))
			quad.scale.x -= 10.0 * (timestep / 1.0f);
		if (input->IsKeyPressed(Xen::KeyCode::KEY_1))
			quad.scale.y += 10.0 * (timestep / 1.0f);
		if (input->IsKeyPressed(Xen::KeyCode::KEY_2))
			quad.scale.y -= 10.0 * (timestep / 1.0f);

		if (input->IsKeyPressed(Xen::KeyCode::KEY_W))
			quad.position.y += 1.0 * (timestep / 1.0f);
		if (input->IsKeyPressed(Xen::KeyCode::KEY_S))
			quad.position.y -= 1.0 * (timestep / 1.0f);
		if (input->IsKeyPressed(Xen::KeyCode::KEY_A))
			quad.position.x -= 1.0 * (timestep / 1.0f);
		if (input->IsKeyPressed(Xen::KeyCode::KEY_D))
			quad.position.x += 1.0 * (timestep / 1.0f);

		if (input->IsKeyPressed(Xen::KeyCode::KEY_UP))
			cam_pos.y += 1.0 * (timestep / 1.0f);
		if (input->IsKeyPressed(Xen::KeyCode::KEY_DOWN))
			cam_pos.y -= 1.0 * (timestep / 1.0f);
		if (input->IsKeyPressed(Xen::KeyCode::KEY_LEFT))
			cam_pos.x -= 1.0 * (timestep / 1.0f);
		if (input->IsKeyPressed(Xen::KeyCode::KEY_RIGHT))
			cam_pos.x += 1.0 * (timestep / 1.0f);

		m_Camera->SetPosition(cam_pos);
		m_Camera->SetScale(cam_zoom);
		m_Camera->Update();
		
		Xen::Renderer2D::BeginScene(m_Camera);

		//for (int i = 0; i < squares; i++)
		//{
		//	for (int j = 0; j < squares; j++)
		//	{
		//		Xen::Renderer2D::DrawClearQuad(Xen::Vec3((float)i, (float)j, 0.0f), 0.0f, Xen::Vec2(square_scale), Xen::Color(color_quad[0] * j, color_quad[1] * i, color_quad[2], color_quad[3]));
		//	}
		//}

		quad.position.z = 0.5f;

		Xen::Renderer2D::DrawTexturedQuad(opengl_logo, Xen::Vec3(0.0f, 0.0f, 0.0f), 0.0f, Xen::Vec2(2.4f, 1.0f), Xen::Color(1.0f, 1.0f, 1.0f, color_quad[3]));
		Xen::Renderer2D::DrawTexturedQuad(tex, Xen::Vec3(-1.0f, 1.0f, 0.0f), 0.0f, Xen::Vec2(square_scale), Xen::Color(color_quad[0], color_quad[1], color_quad[2], color_quad[3]), square_scale);
		Xen::Renderer2D::DrawTexturedQuad(vulkan_logo, Xen::Vec3(quad.position), 0.0f, Xen::Vec2(2.6f, 1.0f), Xen::Color(color_quad[0], color_quad[1], color_quad[2], color_quad[3]));

		Xen::Renderer2D::DrawClearQuad(Xen::Vec3(-1.75f, -1.0f, 0.0f), 0.0f, Xen::Vec2(square_scale), color_gradient_quad_color);
		
		Xen::RenderCommand::Clear();
		Xen::RenderCommand::SetClearColor(Xen::Color(color_bg[0], color_bg[1], color_bg[2], color_bg[3]));
		Xen::Renderer2D::EndScene();
	}

	void OnImGuiUpdate() override
	{
		ImGui::Begin("Color Test");
		ImGui::ColorEdit4("Background Color", color_bg);
		ImGui::ColorEdit4("Quad Color", color_quad);

		ImGui::SliderFloat("Square Scale", &square_scale, 0.0f, 1.0f);
		ImGui::SliderFloat("Red gradient", &a, -1.0f, 1.0f);

		ImGui::SliderInt("No of Squares", &squares, 1, 30);

		ImGui::End();

		//ImGui::ShowMetricsWindow();
	}

	void OnRender() override
	{
		Xen::Renderer2D::RenderFrame();

	}

	void OnWindowResizeEvent(Xen::WindowResizeEvent& evt) override
	{
		Xen::RenderCommand::OnWindowResize(evt.GetWidth(), evt.GetHeight());
		m_Camera->OnWindowResize(evt.GetWidth(), evt.GetHeight());
	}
private:
	Xen::_2D::Quad quad;
	Xen::Ref<Xen::OrthographicCamera> m_Camera;

	Xen::Ref<Xen::Texture2D> tex;
	Xen::Ref<Xen::Texture2D> opengl_logo;
	Xen::Ref<Xen::Texture2D> vulkan_logo;

	Xen::Ref<Xen::Input> input;

	Xen::Vec3 cam_pos;

	float color_bg[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float color_quad[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	float tex_coords[8] = {
		0.5f, 0.5f,
		0.0f, 0.5f,
		0.0f, 0.0f,
		0.5f, 0.0f
	};

	float a = 0.0f;

	Xen::Color color_gradient_quad_color[4] = {
		Xen::Color(1.0f, 0.0f, 0.0f, 1.0f),
		Xen::Color(1.0f, 0.0f, 0.0f, 1.0f),
		Xen::Color(a, 0.0f, 0.0f, 1.0f),
		Xen::Color(a, 0.0f, 0.0f, 1.0f)
	};

	float cam_zoom = 1.0f;
	float square_scale = 0.6f;

	int squares = 1;
};