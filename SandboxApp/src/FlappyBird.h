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
#include <core/renderer/Primitives.h>

#include <random>


class FlappyBirdLayer : public Xen::Layer
{
public:
	FlappyBirdLayer() {}
	virtual ~FlappyBirdLayer() {}

	void OnAttach() override
	{
		m_Camera = std::make_shared<Xen::Camera>(Xen::CameraType::Orthographic, Xen::DesktopApplication::GetWindow()->GetFrameBufferWidth(), Xen::DesktopApplication::GetWindow()->GetFrameBufferHeight());
		Xen::Renderer2D::Init();

		input = Xen::Input::GetInputInterface();
		input->SetWindow(Xen::DesktopApplication::GetWindow());

		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<float> distr(0, 1);

		for (int i = 0; i < 20; i++)
			y_rand[i] = distr(eng);

		qq = 0;

		color = 0.0f;
	}

	void OnDetach() override
	{

	}

	void OnUpdate(double timestep) override
	{
		time = timestep;
		Xen::RenderCommand::Clear();
		Xen::RenderCommand::SetClearColor(Xen::Color(color_bg[0], color_bg[1], color_bg[2], color_bg[3]));

		m_Camera->SetPosition(cam_pos);
		m_Camera->SetScale(cam_zoom);
		m_Camera->Update();

		if (input->IsKeyPressed(Xen::KeyCode::KEY_SPACE))
			pause = 1;

		if (input->IsKeyPressed(Xen::KeyCode::KEY_ESCAPE))
			pause = 0;

		if (input->IsKeyPressed(Xen::KeyCode::KEY_ENTER))
		{
			qq = 0;
			cam_pos.x = 0.0f;
			game_over = 0;
			pause = 0;
			player_y_pos = 0.0f;
		}

		if (pause)
		{
			if (!game_over)
			{
				cam_pos.x += 0.6f * timestep;

				if (input->IsKeyPressed(Xen::KeyCode::KEY_SPACE))
				{
					player_y_pos += 3.0f * timestep;
					d = 0.0f;
				}

				player_y_pos -= d * timestep;
				d += 1.2f * timestep;
			}
		}

		color += 3.0f * timestep;

		color_top[0] = cos(color)	   + 0.3f;
		color_top[1] = cos(color + 60) + 0.3f;
		color_top[2] = cos(color + 30) + 0.3f;

		color_bottom[0] = cos(color)	   + 0.3f;
		color_bottom[1] = cos(color + 90)  + 0.3f;
		color_bottom[2] = cos(color + 200) + 0.3f;

		Xen::Renderer2D::BeginScene(m_Camera, Xen::Vec2(1, 1));
		Xen::Renderer2D::DrawClearCircle(Xen::Vec3(cam_pos.x - 1.4f, player_y_pos, 0.0f), Xen::Vec3(0.0f), Xen::Vec3(0.1f, 0.1f, 1.0f), Xen::Color(color_quad[0], color_quad[1], color_quad[2], color_quad[3]), 1.0f, 0.0f, 0.05f);

		if (c != (int)cam_pos.x - 1)
		{
			c++;
			qq++;

			if (qq >= 15)
				qq = 0;
		}

		for (int i = 0; i < 5; i++)
		{
			Xen::Renderer2D::DrawClearQuad(Xen::Vec3(c + i, y_rand[qq + i] + a, 0.0f), 0.0f, Xen::Vec2(0.1f, 2.0f), Xen::Color(color_top[0], color_top[1], color_top[2], color_top[3]));
			Xen::Renderer2D::DrawClearQuad(Xen::Vec3(c + i, y_rand[qq + i] - a, 0.0f), 0.0f, Xen::Vec2(0.1f, 2.0f), Xen::Color(color_bottom[0], color_bottom[1], color_bottom[2], color_bottom[3]));
		}

		if (IsQuadCollided(Xen::Vec2(cam_pos.x - 1.4f, player_y_pos), Xen::Vec2(0.1f), Xen::Vec2(c, y_rand[qq] + a), Xen::Vec2(0.1f, 2.0f)) 
			|| IsQuadCollided(Xen::Vec2(cam_pos.x - 1.4f, player_y_pos), Xen::Vec2(0.1f), Xen::Vec2(c, y_rand[qq] - a), Xen::Vec2(0.1f, 2.0f)) || player_y_pos > 1.1f || player_y_pos < -1.1f)
		{
			game_over = 1;
		}

		c = (int)cam_pos.x - 1;
		score = cam_pos.x + 0.7f;

		Xen::Renderer2D::EndScene();
	}

	void OnImGuiUpdate() override
	{
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;

		// etc.

		ImVec2 v = ImVec2(600, 600);
		ImGui::SetNextWindowSize(v);

		bool open_ptr = true;
		ImGui::Begin("I'm a Window!", &open_ptr, window_flags);
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 25, 128, 255));
		ImGui::Text("Score: %d", score);

		if (game_over)
		{
			ImGui::Text("Game Over!");
			ImGui::Text("Press Enter to Restart");
		}

		if (!pause)
		{
			ImGui::Text("Press Spacebar to start");
		}

		ImGui::PopStyleColor();

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
		cam_zoom -=  0.1f * evt.GetYOffset();
	}

	bool IsQuadCollided(const Xen::Vec2& position_1, const Xen::Vec2& scale_1, const Xen::Vec2& position_2, const Xen::Vec2& scale_2)
	{
		bool cx, cy;

		if (position_2.x > position_1.x)
			cx = position_2.x - position_1.x <= (scale_1.x + scale_2.x) / 2.0f;
		else
			cx = position_1.x - position_2.x <= (scale_1.x + scale_2.x) / 2.0f;

		if (position_2.y > position_1.y)
			cy = position_2.y - position_1.y <= (scale_1.y + scale_2.y) / 2.0f;
		else
			cy = position_1.y - position_2.y <= (scale_1.y + scale_2.y) / 2.0f;

		return cx && cy;
	}

private:
	Xen::Ref<Xen::Camera> m_Camera;
	Xen::Ref<Xen::Input> input;

	Xen::Vec3 cam_pos;

	float color_bg[4] = { 0.1f, 0.1f, 0.1f, 0.1f };
	float color_quad[4] = { 0.8f, 0.3f, 0.64f, 1.0f };

	float color_top[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float color_bottom[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	float cam_zoom = 1.0f;
	float d = 0.8f;
	float player_y_pos = 0.0f;

	double time;

	float a = 1.250f;

	bool pause = 0;
	
	float y_rand[20];
	float y_[20];

	int c = 0;
	int some = -1;

	int qq;

	int score = 0;

	float color;

	bool game_over = 0;
};