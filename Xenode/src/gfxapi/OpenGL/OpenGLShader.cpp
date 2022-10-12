#include "pch"
#include "OpenGLShader.h"

#include "core/app/Log.h"

#include <glad/gl.h>

namespace Xen {

	// Might not be that efficient!
	// make sure that you insert a extra newline at the end of file!

	OpenGLShader::OpenGLShader(const std::string& filePath)
	{
		std::ifstream stream;
		stream.open(filePath);

		std::stringstream vs, fs;
		bool read_vs = 0;
		bool read_fs = 0;

		while (!stream.eof())
		{
			std::string s;
			std::getline(stream, s);
			

			while (s.contains("#shadertype: "))
			{
				if (s.contains("#shadertype: vertex"))
				{
					readvs:
					while (!read_vs)
					{
						std::string s1;
						std::getline(stream, s1);

						XEN_ENGINE_LOG_INFO(s1);

						if (s1.contains("#shadertype: ") || stream.eof())
						{
							read_vs = 1;
							if (s1.contains("#shadertype: fragment"))
								goto readfs;
						}

						vs << s1 << "\n";

					}
					if (read_vs && read_fs)
					{
						s = "";
					}
				}
				if (s.contains("#shadertype: fragment"))
				{
					readfs:
					while (!read_fs)
					{
						std::string s1;
						std::getline(stream, s1);

						XEN_ENGINE_LOG_INFO(s1);

						if (s1.contains("#shadertype: ") || stream.eof())
						{
							read_fs = 1;
							if (s1.contains("#shadertype: vertex"))
								goto readvs;
						}

						fs << s1 << "\n";

					}
					if (read_vs && read_fs)
					{
						s = "";
					}
				}
			}
		}

		vertexShaderSrc = vs.str();
		fragmentShaderSrc = fs.str();

		m_ShaderID = glCreateProgram();
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