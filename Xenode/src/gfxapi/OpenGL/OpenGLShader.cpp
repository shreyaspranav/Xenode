#include "pch"
#include "OpenGLShader.h"

#include "core/app/Log.h"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include "core/app/Profiler.h"

#include "shaderc/shaderc.hpp"

namespace Xen {

	// Might not be that efficient!
	// make sure that you insert a extra newline at the end of file!

	OpenGLShader::OpenGLShader(const std::string& filePath)
	{
		XEN_PROFILE_FN();

		std::ifstream stream;
		stream.open(filePath);

		std::vector<std::string> shaderCode;
		std::string s;
		while (!stream.eof()) 
		{
			std::getline(stream, s);
			shaderCode.push_back(s);
		}
		
		m_ShaderSrc = PreprocessShaders(shaderCode);

		vertexShaderSrc = m_ShaderSrc.at(GL_VERTEX_SHADER);
		fragmentShaderSrc = m_ShaderSrc.at(GL_FRAGMENT_SHADER);

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

	std::unordered_map<GLenum, std::string> Xen::OpenGLShader::PreprocessShaders(const std::vector<std::string>& shaderCode)
	{
		GLenum shaderType = 0;
		std::stringstream shaderCodeEach;
		bool shaderCodeEachEmpty = true;

		std::unordered_map<GLenum, std::string> shaders;

		for (auto& stringToken : shaderCode) 
		{
			if (stringToken.contains("#shadertype")) 
			{
				if (!shaderCodeEach.str().empty()) {
					shaders.insert({ shaderType, shaderCodeEach.str() });
					shaderCodeEach.str(std::string());
					shaderCodeEachEmpty = true;
				}

				if (stringToken.contains("vertex"))
					shaderType = GL_VERTEX_SHADER;
				else if (stringToken.contains("fragment"))
					shaderType = GL_FRAGMENT_SHADER;
				else if (stringToken.contains("geometry"))
					shaderType = GL_GEOMETRY_SHADER;


				continue;
			}
			XEN_ENGINE_LOG_INFO(stringToken);
			shaderCodeEach << stringToken << "\n";
		}

		if (!shaderCodeEach.str().empty()) {
			shaders.insert({ shaderType, shaderCodeEach.str() });
			shaderCodeEach.str(std::string());
			shaderCodeEachEmpty = true;
		}

		return shaders;
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_ShaderID);
	}
	void OpenGLShader::LoadShader(const VertexBufferLayout& layout)
	{
		XEN_PROFILE_FN();

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
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			XEN_ENGINE_LOG_ERROR("Failed to compile Fragment Shader!");
			XEN_ENGINE_LOG_ERROR(infoLog);
			TRIGGER_BREAKPOINT;
		}

		//for (BufferElement element : layout.GetBufferElements())
		//	glBindAttribLocation(m_ShaderID, element.shader_location, element.name.c_str());

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

	inline void OpenGLShader::Bind() const		{ XEN_PROFILE_FN(); glUseProgram(m_ShaderID); }
	inline void OpenGLShader::Unbind() const	{ glUseProgram(0); }

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		if (m_Uniforms.contains(name))
		{
			glUniform1f(m_Uniforms[name], value);
			return;
		}

		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform1f(m_Uniforms[name], value);
	}
	void OpenGLShader::SetFloat2(const std::string& name, const Vec2& value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniform2f(m_Uniforms[name], value.x, value.y);
			return;
		}

		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform2f(m_Uniforms[name], value.x, value.y);
	}
	void OpenGLShader::SetFloat3(const std::string& name, const Vec3& value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniform3f(m_Uniforms[name], value.x, value.y, value.z);
			return;
		}

		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform3f(m_Uniforms[name], value.x, value.y, value.z);
	}
	void OpenGLShader::SetFloat4(const std::string& name, const Vec4& value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniform4f(m_Uniforms[name], value.x, value.y, value.z, value.w);
			return;
		}

		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform4f(m_Uniforms[name], value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* array, uint32_t count)
	{
		if (m_Uniforms.contains(name))
		{
			glUniform1iv(m_Uniforms[name], count, array);
			return;
		}

		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform1iv(m_Uniforms[name], count, array);

	}
	void OpenGLShader::SetInt(const std::string& name, int value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniform1i(m_Uniforms[name], value);
			return;
		}

		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform1i(m_Uniforms[name], value);
	}
	void OpenGLShader::SetInt2(const std::string& name, const Vec2& value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniform2i(m_Uniforms[name], (int)value.x, (int)value.y);
			return;
		}

		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform2i(m_Uniforms[name], (int)value.x, (int)value.y);
	}
	void OpenGLShader::SetInt3(const std::string& name, const Vec3& value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniform3i(m_Uniforms[name], (int)value.x, (int)value.y, (int)value.z);
			return;
		}

		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform3i(m_Uniforms[name], (int)value.x, (int)value.y, (int)value.z);
	}
	void OpenGLShader::SetInt4(const std::string& name, const Vec4& value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniform4i(m_Uniforms[name], (int)value.x, (int)value.y, (int)value.z, (int)value.w);
			return;
		}

		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform4i(m_Uniforms[name], (int)value.x, (int)value.y, (int)value.z, (int)value.w);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniformMatrix4fv(m_Uniforms[name], 1, GL_FALSE, glm::value_ptr(value));
			return;
		}

		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniformMatrix4fv(m_Uniforms[name], 1, GL_FALSE, glm::value_ptr(value));
	}
}