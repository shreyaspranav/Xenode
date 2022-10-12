#pragma once
#include <Core.h>

#include <core/app/Log.h>
#include <core/app/Layer.h>

#include <core/renderer/Buffer.h>
#include <core/renderer/VertexArray.h>
#include <core/renderer/Shader.h>
#include <core/renderer/Renderer2D.h>

#include <glad/gl.h>

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
	}

	void OnDetach() override
	{
	 
	}

	void OnUpdate(double timestep) override
	{
		int uniform = glGetUniformLocation(m_ShaderID, "u_QuadColor");
		glUniform4fv(uniform, 1, color_quad);

		m_VertexArray->Bind(); 

		//glClear(GL_COLOR_BUFFER_BIT);
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
	}
private:
	Xen::Ref<Xen::FloatBuffer> m_FloatBuffer;
	Xen::Ref<Xen::ElementBuffer> m_ElementBuffer;

	Xen::Ref<Xen::VertexArray> m_VertexArray;

	Xen::Ref<Xen::Shader>  m_Shader;
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
};