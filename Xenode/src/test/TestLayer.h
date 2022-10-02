#pragma once
#include <Core.h>

#include <core/app/Log.h>
#include <core/app/Layer.h>

#include <core/renderer/Buffer.h>

#include <glad/gl.h>

class TestLayer : public Xen::Layer
{
public:
	TestLayer() {}
	virtual ~TestLayer() {}

	void OnAttach() override
	{
        //unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        //glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        //glCompileShader(vertexShader);
        //// check for shader compile errors
        //int success;
        //char infoLog[512];
        //glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        //if (!success)
        //{
        //    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        //    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        //}
        //// fragment shader
        //unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        //glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        //glCompileShader(fragmentShader);
        //// check for shader compile errors
        //glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        //if (!success)
        //{
        //    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        //    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        //}
        //// link shaders
        //shaderProgram = glCreateProgram();
        //glAttachShader(shaderProgram, vertexShader);
        //glAttachShader(shaderProgram, fragmentShader);
        //glLinkProgram(shaderProgram);
        //// check for linking errors
        //glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        //if (!success) {
        //    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        //    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        //}
        //glDeleteShader(vertexShader);
        //glDeleteShader(fragmentShader);

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------


        //unsigned int VBO, VAO, EBO;
        glGenVertexArrays(1, &m_VertexArrayID);
        //glGenBuffers(1, &VBO);
       // glGenBuffers(1, &EBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(m_VertexArrayID);

       // glBindBuffer(GL_ARRAY_BUFFER, VBO);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        m_FloatBuffer = Xen::FloatBuffer::CreateFloatBuffer(9);
        m_FloatBuffer->Put(vertices, 9);

        m_ElementBuffer = Xen::ElementBuffer::CreateElementBuffer(3);
        m_ElementBuffer->Put(indices, 3);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);


        // uncomment this call to draw in wireframe polygons.
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	}

	void OnDetach() override
	{
       
	}

	void OnUpdate(double timestep) override
	{
		XEN_APP_LOG_ERROR(glGetError());
		glBindVertexArray(m_VertexArrayID);
		glEnableVertexAttribArray(0);

        glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(color[0], color[1], color[2], color[3]);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
	}

	void OnImGuiUpdate() override
	{
        ImGui::Begin("Color Test");
        ImGui::ColorEdit4("Background Color", color);
        ImGui::End();
    }
private:
	Xen::Ref<Xen::FloatBuffer> m_FloatBuffer;
	Xen::Ref<Xen::ElementBuffer> m_ElementBuffer;

	float vertices[9] =
	{
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	unsigned int indices[3] = { 0, 1, 2 };

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
    float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};