#include "pch"
#include "OpenGLShader.h"

#include "core/app/Log.h"

#include <glad/gl.h>

namespace Xen {
	OpenGLShader::OpenGLShader(const std::string& filePath)
	{
	}
	OpenGLShader::OpenGLShader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, ShaderType type)
	{
		std::ifstream stream;
		stream.open(vertexShaderFilePath);

		std::stringstream ss;
		while (!stream.eof())
		{
			std::string s;
			std::getline(stream, s);
			ss << s << "\n";
		}

		vertexShaderSrc = ss.str();
		stream.close();

		stream.open(fragmentShaderFilePath);
		ss.str(std::string());

		while (!stream.eof())
		{
			std::string s;
			std::getline(stream, s);
			ss << s << "\n";
		}

		fragmentShaderSrc = ss.str();

		m_ShaderID = glCreateProgram();
	}
	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_ShaderID);
	}
	void OpenGLShader::LoadShader()
	{

		uint32_t vertexShader, fragmentShader;

		const char* vs = vertexShaderSrc.c_str();
		const char* fs = fragmentShaderSrc.c_str();

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vs, NULL);
		glCompileShader(vertexShader);

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fs, NULL);
		glCompileShader(fragmentShader);

		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			XEN_ENGINE_LOG_ERROR("Failed to compile Vertex Shader!");
			XEN_ENGINE_LOG_ERROR(infoLog);
			TRIGGER_BREAKPOINT;
		}

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			XEN_ENGINE_LOG_ERROR("Failed to compile Fragment Shader!");
			XEN_ENGINE_LOG_ERROR(infoLog);
			TRIGGER_BREAKPOINT;
		}

		glAttachShader(m_ShaderID, vertexShader);
		glAttachShader(m_ShaderID, fragmentShader);
		glLinkProgram(m_ShaderID);

		glGetProgramiv(m_ShaderID, GL_LINK_STATUS, &success);

		if (success == GL_FALSE)
		{
			glGetProgramInfoLog(m_ShaderID, 512, NULL, infoLog);
			XEN_ENGINE_LOG_ERROR("Failed to Link Shader Program!");
			XEN_ENGINE_LOG_ERROR(infoLog);
			TRIGGER_BREAKPOINT;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	void OpenGLShader::Bind() const
	{
		glUseProgram(m_ShaderID);
	}
	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}
}