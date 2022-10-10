#pragma once

#include "core/renderer/Shader.h" 

namespace Xen {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, ShaderType type);

		virtual ~OpenGLShader();

		void LoadShader() override;

		inline uint32_t GetShaderID() override { return m_ShaderID; }

		void Bind() const override;
		void Unbind() const override;

	private:
		uint32_t m_ShaderID;

		std::string vertexShaderSrc;
		std::string fragmentShaderSrc;
	};
}

