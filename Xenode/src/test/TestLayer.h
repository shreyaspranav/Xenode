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

class TestLayer : public Xen::Layer
{
public:
	TestLayer() {}
	virtual ~TestLayer() {}

	void OnAttach() override
	{
		m_VertexArray = Xen::VertexArray::GetVertexArray();

		Xen::BufferLayout layout;
		layout.AddBufferElement(Xen::BufferElement("Position", 0, 3, 0, Xen::BufferDataType::Float, false));
		layout.AddBufferElement(Xen::BufferElement("Color", 1, 4, 3, Xen::BufferDataType::Float, false));

		m_Shader = Xen::Shader::CreateShader("assets/shaders/Shader.s");
		m_Shader->LoadShader();
		m_Shader->Bind();

		m_FloatBuffer = Xen::FloatBuffer::CreateFloatBuffer(28);
		m_FloatBuffer->Put(vertices, 28);
		m_FloatBuffer->SetBufferLayout(layout);

		m_ElementBuffer = Xen::ElementBuffer::CreateElementBuffer(6);
		m_ElementBuffer->Put(indices, 6);

		m_VertexArray->SetVertexBuffer(m_FloatBuffer);
		m_VertexArray->SetElementBuffer(m_ElementBuffer);

		m_VertexArray->Load();
		m_ShaderID = m_Shader->GetShaderID();

		m_Camera = std::make_shared<Xen::OrthographicCamera>(Xen::GameApplication::GetWindow()->GetFrameBufferWidth(), Xen::GameApplication::GetWindow()->GetFrameBufferHeight());

		Xen::GameApplication::GetWindow()->GetHeight();

		XEN_ENGINE_LOG_TRACE("{0}, {1}", Xen::GameApplication::GetWindow()->GetHeight(), Xen::GameApplication::GetWindow()->GetFrameBufferHeight());
		input = Xen::Input::GetInputInterface();
		input->SetWindow(Xen::GameApplication::GetWindow());
	}

	void OnDetach() override
	{

	}

	void OnUpdate(double timestep) override
	{
		if (input->IsKeyPressed(Xen::KeyCode::KEY_SPACE))
			quad_scale += 1.0 * timestep;
		if (input->IsKeyPressed(Xen::KeyCode::KEY_TAB))
			quad_scale -= 1.0 * timestep;
		if (input->IsKeyPressed(Xen::KeyCode::KEY_1))
			quad_rotate += 10.0 * timestep;
		if (input->IsKeyPressed(Xen::KeyCode::KEY_2))
			quad_rotate -= 10.0 * timestep;

		if (input->IsKeyPressed(Xen::KeyCode::KEY_W))
			cam_pos.y += 1.0 * timestep;
		if (input->IsKeyPressed(Xen::KeyCode::KEY_S))
			cam_pos.y -= 1.0 * timestep;
		if (input->IsKeyPressed(Xen::KeyCode::KEY_A))
			cam_pos.x += 1.0 * timestep;
		if (input->IsKeyPressed(Xen::KeyCode::KEY_D))
			cam_pos.x -= 1.0 * timestep;

		m_Camera->SetScale(quad_scale);
		m_Camera->SetPosition(cam_pos);
		m_Camera->SetRotation(quad_rotate);

		m_Shader->SetMat4("u_ViewProjectionMatrix", m_Camera->GetViewProjectionMatrix());
		m_Shader->SetFloat4("u_QuadColor", glm::vec4(color_quad[0], color_quad[1], color_quad[2], color_quad[3]));
		m_Camera->Update();

		m_VertexArray->Bind();

		Xen::RenderCommand::Clear();
		Xen::RenderCommand::SetClearColor(Xen::Color(color_bg[0], color_bg[1], color_bg[2], color_bg[3]));

		Xen::Renderer2D::BeginScene();
		Xen::Renderer2D::Submit(m_VertexArray);
		Xen::Renderer2D::EndScene();
	}

	void OnImGuiUpdate() override
	{
		ImGui::Begin("Color Test");
		ImGui::ColorEdit4("Background Color", color_bg);
		ImGui::ColorEdit4("Quad Color", color_quad);
		ImGui::End();

		ImGui::ShowMetricsWindow();
	}

	void OnWindowResizeEvent(Xen::WindowResizeEvent& evt) override
	{
		Xen::RenderCommand::OnWindowResize(evt.GetWidth(), evt.GetHeight());
		m_Camera->OnWindowResize(evt.GetWidth(), evt.GetHeight());
	}

	void OnKeyPressEvent(Xen::KeyPressEvent& evt) override
	{
		if (evt.GetKey() == Xen::KeyCode::KEY_SPACE)
		{
			
		}
	}
private:
	Xen::Ref<Xen::FloatBuffer> m_FloatBuffer;
	Xen::Ref<Xen::ElementBuffer> m_ElementBuffer;

	Xen::Ref<Xen::VertexArray> m_VertexArray;

	Xen::Ref<Xen::Shader> m_Shader;
	Xen::Ref<Xen::OrthographicCamera> m_Camera;
	uint32_t m_ShaderID;

	float vertices[28] =
	{
		 0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, // bottom right
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // bottom left
		-0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f // top left 
	};

	unsigned int indices[6] = 
	{   0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	unsigned int m_VertexArrayID;

	const char* vertexShaderSource = "#version 460 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	const char* fragmentShaderSource = "#version 460 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n\0";

	unsigned int shaderProgram;
	float color_bg[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float color_quad[4] = { 1.0f, 1.0f, 1.0f, 0.0f };

	float quad_scale = 1.0f, quad_rotate = 0.0f;
	glm::vec3 cam_pos = glm::vec3(0.0, 0.0f, 0.0f);

	Xen::Ref<Xen::Input> input;
};