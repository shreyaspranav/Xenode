#pragma once

#include "core/renderer/Shader.h" 

namespace Xen {

	typedef unsigned int GLenum;

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, ShaderType type);

		virtual ~OpenGLShader();

		void LoadShader(const VertexBufferLayout& layout) override;

		inline uint32_t GetShaderID() override { return m_ShaderID; }

		void SetFloat(const std::string& name, float value) override;
		void SetFloat2(const std::string& name, const Vec2& value) override;
		void SetFloat3(const std::string& name, const Vec3& value) override;
		void SetFloat4(const std::string& name, const Vec4& value) override;

		void SetIntArray(const std::string& name, int* array, uint32_t count) override;

		void SetInt(const std::string& name, int value) override;
		void SetInt2(const std::string& name, const Vec2& value) override;
		void SetInt3(const std::string& name, const Vec3& value) override;
		void SetInt4(const std::string& name, const Vec4& value) override;

		void SetMat4(const std::string& name, const glm::mat4& value) override;

		inline void Unbind() const override;

	private:
		inline std::unordered_map<GLenum, std::string> PreprocessShaders(const std::vector<std::string>& shaderCode);

	private:
		uint32_t m_ShaderID;

		std::string vertexShaderSrc;
		inline void Bind() const override;
		std::string fragmentShaderSrc;

		// GL_VERTEX_SHADER: *shader source code*
		// GL_FRAGMENT_SHADER: *shader source code*
		// GL_GEOMETRY_SHADER: *shader source code*
		std::unordered_map<GLenum, std::string> m_ShaderSrc;

		std::unordered_map<std::string, uint32_t> m_Uniforms;
	};
}

