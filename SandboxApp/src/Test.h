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
#include <core/renderer/Camera.h>
#include <core/renderer/Texture.h>
#include <core/renderer/Primitives.h>

class TestLayer : public Xen::Layer
{
public:
	TestLayer() {}
	virtual ~TestLayer() {}

	void OnAttach() override
	{
		m_Camera = std::make_shared<Xen::Camera>(Xen::CameraType::Orthographic, Xen::GameApplication::GetWindow()->GetFrameBufferWidth(), Xen::GameApplication::GetWindow()->GetFrameBufferHeight());
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

		for (int i = 0; i < 32; i++)
		{
			textures.push_back(Xen::Texture2D::CreateTexture2D("assets/textures/opengl.png", 1));
			textures[i]->LoadTexture();
		}

	}

	void OnDetach() override
	{

	}

	void OnUpdate(double timestep) override
	{
		Xen::RenderCommand::Clear();
		Xen::RenderCommand::SetClearColor(Xen::Color(color_bg[0], color_bg[1], color_bg[2], color_bg[3]));
		r += a * timestep;

		color_gradient_quad_color[2].r = a;
		color_gradient_quad_color[3].r = a;

		quad.CalculateVerticesAndIndices();
		if (input->IsKeyPressed(Xen::KeyCode::KEY_SPACE))
			cam_zoom += 1.0 * (timestep / 1.0f) * 3.0f;
		if (input->IsKeyPressed(Xen::KeyCode::KEY_TAB))
			cam_zoom -= 1.0 * (timestep / 1.0f) * 3.0f;

		if (input->IsKeyPressed(Xen::KeyCode::KEY_UP))
			cam_pos.y += 1.0 * (timestep / 1.0f) * 3.0f;
		if (input->IsKeyPressed(Xen::KeyCode::KEY_DOWN))
			cam_pos.y -= 1.0 * (timestep / 1.0f) * 3.0f;
		if (input->IsKeyPressed(Xen::KeyCode::KEY_LEFT))
			cam_pos.x -= 1.0 * (timestep / 1.0f) * 3.0f;
		if (input->IsKeyPressed(Xen::KeyCode::KEY_RIGHT))
			cam_pos.x += 1.0 * (timestep / 1.0f) * 3.0f;

		m_Camera->SetPosition(cam_pos);
		m_Camera->SetScale(cam_zoom);
		m_Camera->Update();

		Xen::Renderer2D::BeginScene(m_Camera);

		//Xen::Renderer2D::DrawTexturedQuad(tex, Xen::Vec3(0.0f, 0.0f, 0.0f), r, Xen::Vec2(1.0f), Xen::Color(1.0f), square_scale);

		Xen::Renderer2D::DrawTexturedQuad(tex, Xen::Vec3(0.0f, 0.0f, 0.0f), r, Xen::Vec2(1000.0f), Xen::Color(1.0f), 20.0f);

		for (int i = 0; i < squares; i++)
		{
			for (int j = 0; j < squares; j++)
			{
				Xen::Renderer2D::DrawClearQuad(Xen::Vec3((float)i, (float)j, z), a, Xen::Vec2(square_scale), Xen::Color(color_quad[0], color_quad[1], color_quad[2], color_quad[3]));
			}
		}
		//for (int j = 0; j < squares; j++)
		//{
		//	for (int i = 0; i < 32; i++)
		//	{
		//		Xen::Renderer2D::DrawTexturedQuad(textures[i], Xen::Vec3((float)i * 3.0f, (float)j, 0.0f), a, Xen::Vec2(square_scale, 1.0f), Xen::Color(color_quad[0] * j, color_quad[1] * i, color_quad[2], color_quad[3]));
		//	}
		//}

		
		Xen::Renderer2D::EndScene();

	}

	void OnImGuiUpdate() override
	{
		ImGui::Begin("Color Test");
		ImGui::ColorEdit4("Background Color", color_bg);
		ImGui::ColorEdit4("Quad Color", color_quad);

		ImGui::SliderFloat("Square Scale", &square_scale, 0.0f, 2.6f);
		ImGui::SliderFloat("Rotation", &a, -180.0f, 180.0f);

		ImGui::SliderFloat("Z Position", &z, -1.0f, 1.0f);

		ImGui::SliderInt("No of Squares", &squares, 1, 1000);

		ImGui::End();

		ImGui::Begin("Renderer Stats");

		ImGui::Text("Draw Calls: %d", Xen::Renderer2D::GetStatistics().draw_calls);
		ImGui::Text("Quads Rendered: %d", Xen::Renderer2D::GetStatistics().quad_count);
		ImGui::Text("Vertex Buffer Size: %d bytes", Xen::Renderer2D::GetStatistics().vertex_buffer_size);
		ImGui::Text("Index Buffer Size: %d bytes", Xen::Renderer2D::GetStatistics().index_buffer_size);
		ImGui::Text("Dynamic batch storage size: %d bytes", Xen::Renderer2D::GetStatistics().predefined_batches);

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
	Xen::Ref<Xen::Camera> m_Camera;

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

	std::vector<Xen::Ref<Xen::Texture2D>> textures;

	Xen::Color color_gradient_quad_color[4] = {
		Xen::Color(1.0f, 0.0f, 0.0f, 1.0f),
		Xen::Color(1.0f, 0.0f, 0.0f, 1.0f),
		Xen::Color(a, 0.0f, 0.0f, 1.0f),
		Xen::Color(a, 0.0f, 0.0f, 1.0f)
	};

	float cam_zoom = 1.0f;
	float square_scale = 0.6f;

	int squares = 1;

	float r = 0.0f;
	float z = 0.0f;
};